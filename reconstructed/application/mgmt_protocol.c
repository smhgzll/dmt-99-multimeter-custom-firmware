/* POTAT-99 management protocol v1, transport independent (see header). */
#include "mgmt_protocol.h"
#include <stddef.h>

enum { CHUNK = 256, HEADER_IN = 8, HEADER_OUT = 9 };
static const uint32_t CODE_FLASH = 0x08000000u, CODE_WINDOW = 0x80000u;

uint32_t mgmt_crc32(uint32_t crc, const uint8_t *p, uint32_t n)
{
    static const uint32_t nibble[16] = {
        0x00000000u, 0x1db71064u, 0x3b6e20c8u, 0x26d930acu, 0x76dc4190u, 0x6b6b51f4u, 0x4db26158u, 0x5005713cu,
        0xedb88320u, 0xf00f9344u, 0xd6d6a3e8u, 0xcb61b38cu, 0x9b64c2b0u, 0x86d3d2d4u, 0xa00ae278u, 0xbdbdf21cu};
    crc = ~crc;
    while (n--) {
        crc ^= *p++;
        crc = (crc >> 4) ^ nibble[crc & 15];
        crc = (crc >> 4) ^ nibble[crc & 15];
    }
    return ~crc;
}

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static void wr16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void wr32(uint8_t *p, uint32_t v) { for (unsigned i = 0; i < 4; i++) p[i] = (uint8_t)(v >> (8 * i)); }

/* ------------------------------------------------------------ responses */
typedef struct { mgmt_state *s; uint32_t crc; bool ok; } writer;

static void out(writer *w, const void *data, uint16_t n)
{
    if (!w->ok || !n) return;
    w->crc = mgmt_crc32(w->crc, data, n);
    w->ok = w->s->b->write(w->s->b->ctx, data, n);
}
static void begin(writer *w, mgmt_state *s, uint8_t cmd, uint16_t seq, uint8_t status, uint16_t len)
{
    uint8_t h[HEADER_OUT] = {0xd9, 0x9b, MGMT_PROTOCOL_VERSION, cmd, 0, 0, status, 0, 0};
    wr16(h + 4, seq);
    wr16(h + 7, len);
    *w = (writer){s, 0, true};
    out(w, h, sizeof h);
}
static void finish(writer *w)
{
    uint8_t c[4];
    wr32(c, w->crc);
    if (w->ok) w->ok = w->s->b->write(w->s->b->ctx, c, 4);
}
static void reply(mgmt_state *s, uint8_t cmd, uint16_t seq, uint8_t status, const void *data, uint16_t n)
{
    writer w;
    begin(&w, s, cmd, seq, status, n);
    out(&w, data, n);
    finish(&w);
}

/* ------------------------------------------------------------ TLV info */
typedef struct { uint8_t *p; uint16_t n, cap; } tlv;
static void tlv_add(tlv *t, uint8_t tag, const void *v, uint8_t len)
{
    if (t->n + 2u + len > t->cap) return;
    t->p[t->n++] = tag;
    t->p[t->n++] = len;
    for (uint8_t i = 0; i < len; i++) t->p[t->n++] = ((const uint8_t *)v)[i];
}
static void tlv_str(tlv *t, uint8_t tag, const char *s)
{
    uint8_t n = 0;
    while (s && s[n] && n < 40) n++;
    tlv_add(t, tag, s, n);
}
static void tlv_u32(tlv *t, uint8_t tag, uint32_t v) { uint8_t b[4]; wr32(b, v); tlv_add(t, tag, b, 4); }
static void tlv_u8(tlv *t, uint8_t tag, uint8_t v) { tlv_add(t, tag, &v, 1); }

enum {
    CAP_SPI_READ = 1u << 0, CAP_SPI_WRITE_ASSETS = 1u << 1, CAP_MEASUREMENT = 1u << 2, CAP_STREAM = 1u << 3,
    CAP_SETTINGS_GET = 1u << 4, CAP_SETTINGS_SET = 1u << 5, CAP_INTERNAL_READ = 1u << 6, CAP_BOOT_READ = 1u << 7,
    CAP_EXPORT = 1u << 8,
};
static uint32_t capabilities(const mgmt_backend *b)
{
    uint32_t c = CAP_EXPORT;
    if (b->spi_read) c |= CAP_SPI_READ;
    if (b->spi_erase_sector && b->spi_program && b->spi_read) c |= CAP_SPI_WRITE_ASSETS;
    if (b->measurement) c |= CAP_MEASUREMENT | CAP_STREAM;
    if (b->settings_get) c |= CAP_SETTINGS_GET;
    if (b->settings_set) c |= CAP_SETTINGS_SET;
    if (b->memory_read) c |= CAP_INTERNAL_READ | CAP_BOOT_READ;
    return c;
}
static uint16_t device_info(mgmt_state *s, uint8_t *buf, uint16_t cap, bool full)
{
    mgmt_device_info i = {0};
    if (s->b->info) s->b->info(s->b->ctx, &i);
    tlv t = {buf, 0, cap};
    tlv_str(&t, 1, i.product);
    tlv_str(&t, 2, i.version);
    tlv_str(&t, 3, i.git);
    tlv_str(&t, 4, i.build_date);
    tlv_u32(&t, 5, i.load_address);
    tlv_u32(&t, 6, i.image_size);
    tlv_u32(&t, 7, i.image_crc32);
    tlv_u8(&t, 13, MGMT_PROTOCOL_VERSION);
    if (!full) return t.n;
    tlv_u32(&t, 8, i.chip_id);
    tlv_add(&t, 9, i.uid, 12);
    tlv_u32(&t, 10, i.flash_kb);
    tlv_u32(&t, 11, i.jedec);
    tlv_u32(&t, 12, i.spi_capacity);
    tlv_u32(&t, 14, capabilities(s->b));
    tlv_u32(&t, 15, MGMT_READ_MAX);
    tlv_u32(&t, 16, MGMT_REQUEST_MAX);
    tlv_u8(&t, 17, i.run_state);
    tlv_u8(&t, 18, i.clock_source);
    tlv_u32(&t, 19, i.uptime_ms);
    tlv_u8(&t, 20, i.charging);
    tlv_u8(&t, 21, i.battery);
    tlv_u8(&t, 22, i.rdp_protected);
    return t.n;
}

/* ---------------------------------------------------------- memory map */
typedef struct { uint8_t space, flags; uint32_t start, length; const char *name; } region;
enum { SPACE_MCU = 0, SPACE_SPI = 1, R_READ = 1, R_WRITE = 2, R_GUARDED = 4 };
static uint16_t memory_map(mgmt_state *s, uint8_t *buf, uint16_t cap)
{
    mgmt_device_info i = {0};
    if (s->b->info) s->b->info(s->b->ctx, &i);
    const region r[] = {
        {SPACE_MCU, R_READ | R_GUARDED, CODE_FLASH, CODE_WINDOW, "code-flash-window"},
        {SPACE_MCU, R_READ | R_GUARDED, CODE_FLASH, i.flash_kb * 1024u, "code-flash-nominal"},
        {SPACE_MCU, R_READ | R_GUARDED, CODE_FLASH, 0x6400u, "vendor-bootloader"},
        {SPACE_MCU, R_READ | R_GUARDED, i.load_address, i.image_size, "application"},
        {SPACE_MCU, R_READ | R_GUARDED, 0x08040000u, 256u, "settings-page"},
        {SPACE_MCU, R_READ | R_GUARDED, 0x1fff8000u, 0x7000u, "system-boot-area"},
        {SPACE_MCU, R_READ | R_GUARDED, 0x1ffff7e0u, 32u, "esig"},
        {SPACE_MCU, R_READ | R_GUARDED, 0x1ffff800u, 16u, "option-bytes"},
        {SPACE_MCU, 0, 0x20000000u, 0x8000u, "sram"},
        {SPACE_SPI, R_READ, 0, i.spi_capacity, "winbond"},
        {SPACE_SPI, R_READ, 0, 63u * MGMT_SPI_SECTOR, "mbr-reserved"},
        {SPACE_SPI, R_READ, 63u * MGMT_SPI_SECTOR, 400u * MGMT_SPI_SECTOR, "boot-fat12"},
        {SPACE_SPI, R_READ | R_WRITE, MGMT_ASSET_FIRST_SECTOR * MGMT_SPI_SECTOR, MGMT_ASSET_SECTORS * MGMT_SPI_SECTOR,
         "assets-fat12"},
    };
    uint16_t n = 0;
    for (size_t k = 0; k < sizeof r / sizeof r[0]; k++) {
        uint8_t len = 0;
        while (r[k].name[len]) len++;
        if (n + 11u + len > cap) break;
        buf[n++] = r[k].space;
        buf[n++] = r[k].flags;
        wr32(buf + n, r[k].start); n += 4;
        wr32(buf + n, r[k].length); n += 4;
        buf[n++] = len;
        for (uint8_t c = 0; c < len; c++) buf[n++] = (uint8_t)r[k].name[c];
    }
    return n;
}

/* ------------------------------------------------------------ handlers */
static void read_spi(mgmt_state *s, uint8_t cmd, uint16_t seq, uint32_t address, uint16_t n)
{
    uint8_t chunk[CHUNK];
    writer w;
    begin(&w, s, cmd, seq, MGMT_OK, n);
    while (n && w.ok) {
        uint16_t k = n > CHUNK ? CHUNK : n;
        if (!s->b->spi_read(s->b->ctx, address, chunk, k))
            for (uint16_t i = 0; i < k; i++) chunk[i] = 0;
        out(&w, chunk, k);
        address += k;
        n = (uint16_t)(n - k);
    }
    finish(&w);
}

static void read_memory(mgmt_state *s, uint8_t cmd, uint16_t seq, uint32_t address, uint16_t n)
{
    uint8_t chunk[CHUNK];
    /* Pass 1 proves the whole range readable, so the status is known before data is sent. */
    for (uint32_t at = 0; at < n; at += CHUNK) {
        uint16_t k = (uint16_t)(n - at > CHUNK ? CHUNK : n - at);
        uint8_t st = s->b->memory_read(s->b->ctx, address + at, chunk, k);
        if (st != MGMT_OK) { reply(s, cmd, seq, st, 0, 0); return; }
    }
    writer w;
    begin(&w, s, cmd, seq, MGMT_OK, n);
    for (uint32_t at = 0; at < n && w.ok; at += CHUNK) {
        uint16_t k = (uint16_t)(n - at > CHUNK ? CHUNK : n - at);
        if (s->b->memory_read(s->b->ctx, address + at, chunk, k) != MGMT_OK)
            for (uint16_t i = 0; i < k; i++) chunk[i] = 0;
        out(&w, chunk, k);
    }
    finish(&w);
}

static bool in_range(uint32_t start, uint32_t len, uint32_t lo, uint32_t size)
{
    return len && start >= lo && start - lo <= size && len <= size - (start - lo);
}

static uint16_t encode_measurement(const mgmt_measurement *m, uint8_t *b)
{
    wr32(b, m->frame_seq);
    wr32(b + 4, m->frame_age_ms);
    const uint8_t bytes[12] = {m->run_state, m->view, m->layout, m->mode, m->unit, m->numeric, m->hold, m->relative,
                               m->ac_dc, m->status_high, m->charging, m->battery};
    for (unsigned i = 0; i < 12; i++) b[8 + i] = bytes[i];
    union { float f; uint32_t u; } v = {m->value}, r = {m->relative_value};
    wr32(b + 20, v.u);
    wr32(b + 24, r.u);
    for (unsigned i = 0; i < 8; i++) b[28 + i] = (uint8_t)m->text[i];
    for (unsigned i = 0; i < 13; i++) b[36 + i] = m->frame[i];
    b[49] = 0;
    return 50;
}

static void end_session(mgmt_state *s)
{
    if (!s->session) return;
    s->session = false;
    s->erased_sector = -1;
    if (s->b->maintenance) s->b->maintenance(s->b->ctx, false);
}

static void write_assets(mgmt_state *s, uint16_t seq, const uint8_t *p, uint16_t n)
{
    const uint8_t cmd = MGMT_WRITE_SPI_ASSET_REGION;
    if (!(capabilities(s->b) & CAP_SPI_WRITE_ASSETS)) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
    if (n < 1) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
    uint32_t now = s->b->now_ms(s->b->ctx);
    if (p[0] == MGMT_WRITE_BEGIN) {
        if (n != 5 || p[1] != 'A' || p[2] != 'S' || p[3] != 'E' || p[4] != 'T') { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        if (s->session) { reply(s, cmd, seq, MGMT_E_BUSY, 0, 0); return; }
        s->session = true;
        s->token = (now * 2654435761u) ^ 0x44533939u;
        if (!s->token) s->token = 1;
        s->erased_sector = -1;
        s->session_seen = now;
        if (s->b->maintenance) s->b->maintenance(s->b->ctx, true);
        uint8_t r[8];
        wr32(r, s->token);
        wr16(r + 4, MGMT_ASSET_FIRST_SECTOR);
        wr16(r + 6, MGMT_ASSET_SECTORS);
        reply(s, cmd, seq, MGMT_OK, r, sizeof r);
        return;
    }
    if (n < 5 || !s->session || rd32(p + 1) != s->token) { reply(s, cmd, seq, MGMT_E_SESSION, 0, 0); return; }
    s->session_seen = now;
    if (p[0] == MGMT_WRITE_ERASE) {
        if (n != 7) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        uint16_t sector = rd16(p + 5);
        if (sector < MGMT_ASSET_FIRST_SECTOR || sector >= MGMT_ASSET_FIRST_SECTOR + MGMT_ASSET_SECTORS) {
            reply(s, cmd, seq, MGMT_E_DENIED, 0, 0);
            return;
        }
        s->erased_sector = -1;
        if (!s->b->spi_erase_sector(s->b->ctx, sector)) { reply(s, cmd, seq, MGMT_E_IO, 0, 0); return; }
        s->erased_sector = sector;
        reply(s, cmd, seq, MGMT_OK, 0, 0);
        return;
    }
    if (p[0] == MGMT_WRITE_PROGRAM) {
        if (n < 10 || n - 9u > MGMT_PROGRAM_MAX) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        uint32_t address = rd32(p + 5);
        uint16_t len = (uint16_t)(n - 9);
        if (s->erased_sector < 0 || address / MGMT_SPI_SECTOR != (uint32_t)s->erased_sector ||
            (address + len - 1) / MGMT_SPI_SECTOR != (uint32_t)s->erased_sector || (address & 255u) + len > 256u) {
            reply(s, cmd, seq, MGMT_E_DENIED, 0, 0);
            return;
        }
        if (!s->b->spi_program(s->b->ctx, address, p + 9, len)) { reply(s, cmd, seq, MGMT_E_IO, 0, 0); return; }
        uint8_t check[MGMT_PROGRAM_MAX];
        if (!s->b->spi_read(s->b->ctx, address, check, len)) { reply(s, cmd, seq, MGMT_E_IO, 0, 0); return; }
        for (uint16_t i = 0; i < len; i++)
            if (check[i] != p[9 + i]) { reply(s, cmd, seq, MGMT_E_VERIFY, 0, 0); return; }
        reply(s, cmd, seq, MGMT_OK, 0, 0);
        return;
    }
    if (p[0] == MGMT_WRITE_END) {
        end_session(s);
        reply(s, cmd, seq, MGMT_OK, 0, 0);
        return;
    }
    reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0);
}

static void dispatch(mgmt_state *s, uint8_t cmd, uint16_t seq, const uint8_t *p, uint16_t n)
{
    uint8_t buf[200];
    mgmt_device_info info = {0};
    switch (cmd) {
    case MGMT_PING:
        reply(s, cmd, seq, MGMT_OK, p, n);
        return;
    case MGMT_GET_DEVICE_INFO:
        reply(s, cmd, seq, MGMT_OK, buf, device_info(s, buf, sizeof buf, true));
        return;
    case MGMT_GET_FIRMWARE_VERSION:
        reply(s, cmd, seq, MGMT_OK, buf, device_info(s, buf, sizeof buf, false));
        return;
    case MGMT_GET_CAPABILITIES: {
        wr32(buf, capabilities(s->b));
        wr16(buf + 4, MGMT_READ_MAX);
        wr16(buf + 6, MGMT_REQUEST_MAX);
        wr16(buf + 8, MGMT_PROGRAM_MAX);
        buf[10] = MGMT_PROTOCOL_VERSION;
        reply(s, cmd, seq, MGMT_OK, buf, 11);
        return;
    }
    case MGMT_GET_MEMORY_MAP: {
        uint8_t map[400];
        reply(s, cmd, seq, MGMT_OK, map, memory_map(s, map, sizeof map));
        return;
    }
    case MGMT_GET_SPI_FLASH_ID:
        if (s->b->info) s->b->info(s->b->ctx, &info);
        wr32(buf, info.jedec);
        wr32(buf + 4, info.spi_capacity);
        reply(s, cmd, seq, MGMT_OK, buf, 8);
        return;
    case MGMT_READ_SPI_FLASH:
    case MGMT_READ_SPI_PARTITION: {
        if (!s->b->spi_read) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (s->b->info) s->b->info(s->b->ctx, &info);
        uint32_t lo = 0, size = info.spi_capacity, address;
        uint16_t len;
        if (cmd == MGMT_READ_SPI_FLASH) {
            if (n != 6) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
            address = rd32(p);
            len = rd16(p + 4);
        } else {
            static const uint16_t first[3] = {0, 63, MGMT_ASSET_FIRST_SECTOR}, count[3] = {63, 400, MGMT_ASSET_SECTORS};
            if (n != 7 || p[0] > 2) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
            lo = first[p[0]] * MGMT_SPI_SECTOR;
            size = count[p[0]] * MGMT_SPI_SECTOR;
            address = lo + rd32(p + 1);
            len = rd16(p + 5);
            if (rd32(p + 1) >= size) { reply(s, cmd, seq, MGMT_E_RANGE, 0, 0); return; }
        }
        if (len > MGMT_READ_MAX || !in_range(address, len, lo, size)) { reply(s, cmd, seq, MGMT_E_RANGE, 0, 0); return; }
        read_spi(s, cmd, seq, address, len);
        return;
    }
    case MGMT_VERIFY_SPI_DATA: {
        if (!s->b->spi_read) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (n != 8) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        if (s->b->info) s->b->info(s->b->ctx, &info);
        uint32_t address = rd32(p), len = rd32(p + 4), crc = 0;
        if (!in_range(address, len, 0, info.spi_capacity)) { reply(s, cmd, seq, MGMT_E_RANGE, 0, 0); return; }
        uint8_t chunk[CHUNK];
        for (uint32_t at = 0; at < len; at += CHUNK) {
            uint16_t k = (uint16_t)(len - at > CHUNK ? CHUNK : len - at);
            if (!s->b->spi_read(s->b->ctx, address + at, chunk, k)) { reply(s, cmd, seq, MGMT_E_IO, 0, 0); return; }
            crc = mgmt_crc32(crc, chunk, k);
        }
        wr32(buf, crc);
        wr32(buf + 4, len);
        reply(s, cmd, seq, MGMT_OK, buf, 8);
        return;
    }
    case MGMT_WRITE_SPI_ASSET_REGION:
        write_assets(s, seq, p, n);
        return;
    case MGMT_GET_CURRENT_MEASUREMENT: {
        mgmt_measurement m = {0};
        if (!s->b->measurement) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        bool ok = s->b->measurement(s->b->ctx, &m);
        uint16_t len = encode_measurement(&m, buf);
        reply(s, cmd, seq, ok ? MGMT_OK : MGMT_E_UNAVAILABLE, buf, len);
        return;
    }
    case MGMT_START_MEASUREMENT_STREAM:
        if (!s->b->measurement) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (n != 2 || (rd16(p) && (rd16(p) < 50 || rd16(p) > 5000))) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        s->stream = true;
        s->interval_ms = rd16(p);
        s->last_frame_seq = 0xffffffffu;
        s->last_event_ms = s->b->now_ms(s->b->ctx) - 100000u;
        reply(s, cmd, seq, MGMT_OK, 0, 0);
        return;
    case MGMT_STOP_MEASUREMENT_STREAM:
        s->stream = false;
        reply(s, cmd, seq, MGMT_OK, 0, 0);
        return;
    case MGMT_GET_SETTINGS: {
        if (!s->b->settings_get) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        uint8_t settings[256];
        s->b->settings_get(s->b->ctx, settings);
        reply(s, cmd, seq, MGMT_OK, settings, sizeof settings);
        return;
    }
    case MGMT_SET_SETTINGS:
        if (!s->b->settings_set) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (n != 3) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        reply(s, cmd, seq, s->b->settings_set(s->b->ctx, p[0], rd16(p + 1)), 0, 0);
        return;
    case MGMT_READ_ACCESSIBLE_INTERNAL_FLASH: {
        if (!s->b->memory_read) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (n != 6) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        uint32_t address = rd32(p);
        uint16_t len = rd16(p + 4);
        if (len > MGMT_READ_MAX || !in_range(address, len, CODE_FLASH, CODE_WINDOW)) {
            reply(s, cmd, seq, MGMT_E_RANGE, 0, 0);
            return;
        }
        read_memory(s, cmd, seq, address, len);
        return;
    }
    case MGMT_READ_ACCESSIBLE_BOOT_REGION: {
        static const uint32_t start[4] = {0x08000000u, 0x1fff8000u, 0x1ffff800u, 0x1ffff7e0u};
        static const uint32_t size[4] = {0x6400u, 0x7000u, 16u, 32u};
        if (!s->b->memory_read) { reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0); return; }
        if (n != 7 || p[0] > 3) { reply(s, cmd, seq, MGMT_E_ARGUMENT, 0, 0); return; }
        uint32_t offset = rd32(p + 1);
        uint16_t len = rd16(p + 5);
        if (len > MGMT_READ_MAX || offset >= size[p[0]] || !in_range(start[p[0]] + offset, len, start[p[0]], size[p[0]])) {
            reply(s, cmd, seq, MGMT_E_RANGE, 0, 0);
            return;
        }
        read_memory(s, cmd, seq, start[p[0]] + offset, len);
        return;
    }
    case MGMT_EXPORT_CONFIGURATION: {
        /* Sections: id u8, length u16, data. 1 device info TLV, 2 settings image,
         * 3 option bytes, 4 ESIG, 5 memory map. Unreadable sections are omitted. */
        uint8_t settings[256], ob[48];
        uint16_t info_len = device_info(s, buf, sizeof buf, true);
        uint16_t total = (uint16_t)(3 + info_len);
        bool have_settings = s->b->settings_get != 0, have_ob = false, have_esig = false;
        if (have_settings) { s->b->settings_get(s->b->ctx, settings); total += 3 + 256; }
        if (s->b->memory_read) {
            have_ob = s->b->memory_read(s->b->ctx, 0x1ffff800u, ob, 16) == MGMT_OK;
            have_esig = s->b->memory_read(s->b->ctx, 0x1ffff7e0u, ob + 16, 32) == MGMT_OK;
            total += (uint16_t)((have_ob ? 3 + 16 : 0) + (have_esig ? 3 + 32 : 0));
        }
        uint8_t map[400];
        uint16_t map_len = memory_map(s, map, sizeof map);
        total += 3 + map_len;
        writer w;
        begin(&w, s, cmd, seq, MGMT_OK, total);
        uint8_t h[3] = {1, 0, 0};
        wr16(h + 1, info_len); out(&w, h, 3); out(&w, buf, info_len);
        if (have_settings) { h[0] = 2; wr16(h + 1, 256); out(&w, h, 3); out(&w, settings, 256); }
        if (have_ob) { h[0] = 3; wr16(h + 1, 16); out(&w, h, 3); out(&w, ob, 16); }
        if (have_esig) { h[0] = 4; wr16(h + 1, 32); out(&w, h, 3); out(&w, ob + 16, 32); }
        h[0] = 5; wr16(h + 1, map_len); out(&w, h, 3); out(&w, map, map_len);
        finish(&w);
        return;
    }
    default:
        reply(s, cmd, seq, MGMT_E_COMMAND, 0, 0);
        return;
    }
}

/* -------------------------------------------------------------- parser */
void mgmt_init(mgmt_state *s, const mgmt_backend *b)
{
    *s = (mgmt_state){0};
    s->b = b;
    s->erased_sector = -1;
}

static void frame_done(mgmt_state *s)
{
    uint16_t len = rd16(s->rx + 6);
    uint32_t crc = mgmt_crc32(0, s->rx, HEADER_IN + len);
    uint8_t cmd = s->rx[3];
    uint16_t seq = rd16(s->rx + 4);
    s->rx_len = 0;
    if (crc != rd32(s->rx + HEADER_IN + len)) {
        s->frames_bad++;
        reply(s, cmd, seq, MGMT_E_FRAME, 0, 0);
        return;
    }
    s->frames_ok++;
    dispatch(s, cmd, seq, s->rx + HEADER_IN, len);
}

void mgmt_receive(mgmt_state *s, const uint8_t *data, uint16_t n)
{
    uint32_t now = s->b->now_ms(s->b->ctx);
    if (s->rx_len && now - s->rx_started > MGMT_FRAME_TIMEOUT_MS) s->rx_len = 0;
    for (uint16_t i = 0; i < n; i++) {
        uint8_t c = data[i];
        if (s->rx_len == 0) {
            if (c == 0xd9) { s->rx[s->rx_len++] = c; s->rx_started = now; }
            continue;
        }
        if (s->rx_len == 1) {
            if (c == 0x9a) s->rx[s->rx_len++] = c;
            else s->rx_len = c == 0xd9 ? 1 : 0;
            continue;
        }
        s->rx[s->rx_len++] = c;
        if (s->rx_len == HEADER_IN) {
            uint16_t len = rd16(s->rx + 6);
            if (s->rx[2] != MGMT_PROTOCOL_VERSION || len > MGMT_REQUEST_MAX) {
                s->frames_bad++;
                s->rx_len = 0;
                reply(s, s->rx[3], rd16(s->rx + 4), MGMT_E_FRAME, 0, 0);
            }
            continue;
        }
        if (s->rx_len >= HEADER_IN && s->rx_len == HEADER_IN + rd16(s->rx + 6) + 4u) frame_done(s);
    }
}

void mgmt_poll(mgmt_state *s)
{
    uint32_t now = s->b->now_ms(s->b->ctx);
    if (s->session && now - s->session_seen > MGMT_SESSION_TIMEOUT_MS) end_session(s);
    if (!s->stream || !s->b->measurement) return;
    mgmt_measurement m = {0};
    bool ok = s->b->measurement(s->b->ctx, &m);
    uint32_t wait = s->interval_ms ? s->interval_ms : 0;
    if (ok && m.frame_seq == s->last_frame_seq) return;
    if (!ok && now - s->last_event_ms < 1000u) return;
    if (ok && now - s->last_event_ms < wait) return;
    uint8_t buf[50];
    uint16_t len = encode_measurement(&m, buf);
    s->last_frame_seq = m.frame_seq;
    s->last_event_ms = now;
    reply(s, MGMT_MEASUREMENT_EVENT, s->event_seq++, ok ? MGMT_OK : MGMT_E_UNAVAILABLE, buf, len);
}

bool mgmt_maintenance_active(const mgmt_state *s) { return s->session; }
