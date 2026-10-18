/* Simulated device for DMT99 Studio: serves the management protocol
 * (application/mgmt_protocol.c) on a pseudo terminal, backed by the
 * simulator state. Host-only; the meter firmware does not include it.
 *
 * Data sources are explicit and never invented:
 *  - Winbond: the simulator's in-memory copy of the --flash image (writes
 *    change that copy only; the image file is never written);
 *  - measurement: the controller state fed by script/stdin commands
 *    (ack, reading, packet);
 *  - MCU memory: the application BIN given with --mcu-image (at 0x08006400)
 *    and the settings page; every other address answers MGMT_E_FAULT, so the
 *    unread bootloader is never fabricated. */
#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include "device_link.h"
#include "config.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef DMT_FW_VERSION
#define DMT_FW_VERSION "sim"
#endif

static uint32_t now_ms(void *p)
{
    (void)p;
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint32_t)(t.tv_sec * 1000u + t.tv_nsec / 1000000);
}
static bool link_write(void *p, const uint8_t *data, uint16_t n)
{
    device_link *d = p;
    while (n) {
        ssize_t k = write(d->master, data, n);
        if (k < 0 && errno == EINTR) continue;
        if (k < 0 && errno == EAGAIN) { struct pollfd f = {d->master, POLLOUT, 0}; if (poll(&f, 1, 1000) <= 0) return false; continue; }
        if (k <= 0) return false;
        data += k;
        n = (uint16_t)(n - k);
    }
    return true;
}
static void info(void *p, mgmt_device_info *i)
{
    device_link *d = p;
    *i = (mgmt_device_info){0};
    i->product = "POTAT-99 (simulated)";
    i->version = DMT_FW_VERSION;
    i->git = "simulator";
    i->build_date = __DATE__;
    i->load_address = 0x08006400u;
    i->image_size = (uint32_t)d->mcu_image_size;
    i->image_crc32 = d->mcu_image ? mgmt_crc32(0, d->mcu_image, (uint32_t)d->mcu_image_size) : 0;
    i->jedec = 0xef4017u;               /* W25Q64JV, as the meter reports */
    i->spi_capacity = 8388608u;
    i->uptime_ms = now_ms(p) - d->started;
    i->run_state = d->sim->app.run;
    i->charging = d->sim->charging;
    i->battery = d->sim->battery;
}
static bool spi_read(void *p, uint32_t a, uint8_t *out, uint16_t n)
{
    device_link *d = p;
    if (a > 8388608u || n > 8388608u - a) return false;
    memcpy(out, d->sim->flash + a, n);
    return true;
}
static bool spi_erase(void *p, uint32_t sector)
{
    device_link *d = p;
    if (sector >= 2048u) return false;
    memset(d->sim->flash + sector * 4096u, 0xff, 4096);
    d->erases++;
    return true;
}
static bool spi_program(void *p, uint32_t a, const uint8_t *data, uint16_t n)
{
    device_link *d = p;
    if (a > 8388608u || n > 8388608u - a) return false;
    for (uint16_t i = 0; i < n; i++) d->sim->flash[a + i] &= data[i];   /* NOR: program clears bits */
    d->programs++;
    return true;
}
static uint8_t memory_read(void *p, uint32_t a, uint8_t *out, uint16_t n)
{
    device_link *d = p;
    const uint32_t app = 0x08006400u, settings = 0x08040000u;
    if (d->mcu_image && a >= app && a - app <= d->mcu_image_size && n <= d->mcu_image_size - (a - app)) {
        memcpy(out, d->mcu_image + (a - app), n);
        return MGMT_OK;
    }
    if (a >= settings && a - settings <= 256u && n <= 256u - (a - settings)) {
        for (uint16_t i = 0; i < n; i++) {
            uint32_t k = a - settings + i;
            uint16_t h = d->sim->app.settings.halfwords[k / 2];
            out[i] = (uint8_t)(k & 1 ? h >> 8 : h);
        }
        return MGMT_OK;
    }
    return MGMT_E_FAULT;
}
static bool measurement(void *p, mgmt_measurement *m)
{
    device_link *d = p;
    const dmt_control *c = &d->sim->app.control;
    *m = (mgmt_measurement){0};
    m->frame_seq = d->sim->packets;
    m->frame_age_ms = now_ms(p) - d->last_packet_ms;
    m->run_state = d->sim->app.run;
    m->view = c->view;
    m->layout = c->layout;
    m->mode = c->mode;
    m->unit = c->unit;
    m->numeric = c->numeric;
    m->hold = c->hold;
    m->relative = c->relative;
    m->ac_dc = c->ac_dc_code;
    m->status_high = c->status_high;
    m->charging = d->sim->charging;
    m->battery = d->sim->battery;
    m->value = c->value;
    m->relative_value = c->relative_value;
    for (unsigned i = 0; i < sizeof m->text && i < sizeof c->text; i++) m->text[i] = c->text[i];
    memcpy(m->frame, d->sim->app.frame, 13);
    return d->sim->power && d->sim->app.run == DMT_RUNNING && d->sim->packets != 0;
}
static void settings_get(void *p, uint8_t out[256])
{
    device_link *d = p;
    for (unsigned i = 0; i < 128; i++) {
        out[2 * i] = (uint8_t)d->sim->app.settings.halfwords[i];
        out[2 * i + 1] = (uint8_t)(d->sim->app.settings.halfwords[i] >> 8);
    }
}
static uint8_t settings_set(void *p, uint8_t field, uint16_t value)
{
    device_link *d = p;
    uint16_t *w = settings_image_word(&d->sim->app.settings);
    switch (field) {
    case MGMT_SET_LANGUAGE: if (value > 1) return MGMT_E_ARGUMENT; settings_language_set(w, value); return MGMT_OK;
    case MGMT_SET_BACKLIGHT: if (value > 3) return MGMT_E_ARGUMENT; *w = (uint16_t)((*w & ~0x18u) | value << 3); return MGMT_OK;
    case MGMT_SET_FAHRENHEIT: if (value > 1) return MGMT_E_ARGUMENT; *w = (uint16_t)((*w & ~0x20u) | value << 5); return MGMT_OK;
    case MGMT_SET_AUTO_OFF: if (value > 3) return MGMT_E_ARGUMENT; settings_auto_off_set(w, value); return MGMT_OK;
    case MGMT_SET_VIEW: if (value < 1 || value > 3) return MGMT_E_ARGUMENT; settings_access(w, SETTINGS_THEME, 1, (int16_t)value); return MGMT_OK;
    case MGMT_SET_LAYOUT: if (value > 1) return MGMT_E_ARGUMENT; settings_access(w, SETTINGS_LAYOUT, 1, (int16_t)value); return MGMT_OK;
    default: return MGMT_E_ARGUMENT;
    }
}
static void maintenance(void *p, bool on)
{
    device_link *d = p;
    d->maintenance = on;
    if (!on) {
        /* Drop FatFs caches and repaint from the updated image. */
        filesystem_mount(&d->sim->filesystem, 1);
        simulator_power_on(d->sim);
    }
}

bool device_link_open(device_link *d, dmt_simulator *sim, const char *mcu_image)
{
    memset(d, 0, sizeof *d);
    d->sim = sim;
    d->master = d->slave = -1;
    if (mcu_image) {
        FILE *f = fopen(mcu_image, "rb");
        if (!f) return false;
        fseek(f, 0, SEEK_END);
        long n = ftell(f);
        fseek(f, 0, SEEK_SET);
        d->mcu_image = n > 0 && n <= 0x40000 ? malloc((size_t)n) : 0;
        bool ok = d->mcu_image && fread(d->mcu_image, 1, (size_t)n, f) == (size_t)n;
        fclose(f);
        if (!ok) { free(d->mcu_image); return false; }
        d->mcu_image_size = (size_t)n;
    }
    d->master = posix_openpt(O_RDWR | O_NOCTTY);
    if (d->master < 0 || grantpt(d->master) || unlockpt(d->master)) return false;
    const char *name = ptsname(d->master);
    if (!name) return false;
    snprintf(d->path, sizeof d->path, "%s", name);
    /* Keep a slave descriptor open (no EIO between host sessions) and raw. */
    d->slave = open(d->path, O_RDWR | O_NOCTTY);
    if (d->slave < 0) return false;
    struct termios t;
    if (tcgetattr(d->slave, &t) == 0) { cfmakeraw(&t); tcsetattr(d->slave, TCSANOW, &t); }
    fcntl(d->master, F_SETFL, fcntl(d->master, F_GETFL) | O_NONBLOCK);
    d->started = now_ms(0);
    d->backend = (mgmt_backend){d, now_ms, link_write, info, spi_read, spi_erase, spi_program, memory_read,
                                measurement, settings_get, settings_set, maintenance};
    mgmt_init(&d->state, &d->backend);
    return true;
}

void device_link_service(device_link *d, int timeout_ms, int stdin_fd, bool *stdin_ready)
{
    struct pollfd f[2] = {{d->master, POLLIN, 0}, {stdin_fd, POLLIN, 0}};
    *stdin_ready = false;
    int n = poll(f, stdin_fd >= 0 ? 2 : 1, timeout_ms);
    if (n > 0 && (f[0].revents & POLLIN)) {
        uint8_t buf[512];
        ssize_t k = read(d->master, buf, sizeof buf);
        if (k > 0) mgmt_receive(&d->state, buf, (uint16_t)k);
    }
    if (n > 0 && stdin_fd >= 0 && (f[1].revents & (POLLIN | POLLHUP))) *stdin_ready = true;
    mgmt_poll(&d->state);
}

void device_link_packet(device_link *d) { d->last_packet_ms = now_ms(0); }

void device_link_close(device_link *d)
{
    if (d->slave >= 0) close(d->slave);
    if (d->master >= 0) close(d->master);
    free(d->mcu_image);
    d->mcu_image = 0;
}
