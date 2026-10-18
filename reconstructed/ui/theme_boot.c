/* Boot-time theme installation from the bootloader's USB disk.
 *
 * The user copies THEME.PKG to the disk the manufacturer bootloader shows
 * (POWER+MENU); that disk is FAT12 volume 0 of the Winbond. One FATFS object
 * serves both volumes, so the package is located on volume 0 (start cluster,
 * size, FAT geometry), volume 1 is mounted again and the package bytes are
 * read raw through volume 0's FAT12 chain while the theme is written to
 * volume 1. The package is deleted afterwards, also when it was rejected,
 * so a bad file is not retried at every start. */
#include "theme_ui.h"

#define C_BG 0x0000u
#define C_TEXT 0xffffu
#define C_ACCENT 0x1c9fu
#define C_GOOD 0x07e0u
#define C_BAD 0xf800u

typedef struct {
    uint32_t fatbase, database, csize, n_fatent, start, size;
    uint32_t cached_index, cached_cluster;
} raw_package;

static bool fat12_next(const raw_package *p, uint32_t cluster, uint32_t *next)
{
    uint8_t b[2];
    if (cluster < 2 || cluster >= p->n_fatent) return false;
    if (filesystem_raw_read(p->fatbase * 4096u + cluster + cluster / 2, b, 2)) return false;
    uint32_t v = (uint32_t)(b[0] | b[1] << 8);
    *next = cluster & 1 ? v >> 4 : v & 0xfff;
    return *next >= 2 && *next < p->n_fatent;
}

static bool raw_read(void *context, uint32_t offset, uint8_t *out, uint32_t n)
{
    raw_package *p = context;
    uint32_t cluster_bytes = p->csize * 4096u;
    while (n) {
        uint32_t index = offset / cluster_bytes, within = offset % cluster_bytes;
        if (index < p->cached_index) { p->cached_index = 0; p->cached_cluster = p->start; }
        while (p->cached_index < index) {
            uint32_t next;
            if (!fat12_next(p, p->cached_cluster, &next)) return false;
            p->cached_cluster = next;
            p->cached_index++;
        }
        uint32_t k = cluster_bytes - within < n ? cluster_bytes - within : n;
        uint32_t at = (p->database + (p->cached_cluster - 2) * p->csize) * 4096u + within;
        if (filesystem_raw_read(at, out, k)) return false;
        out += k;
        offset += k;
        n -= k;
    }
    return true;
}

static void text(const dmt_canvas *c, uint16_t x, uint16_t y, uint8_t size, uint16_t color, const char *s)
{
    if (c) graphics_ascii_text(c, x, y, s, size, color, C_BG, false);
}
static void number(char *out, uint32_t v)
{
    char t[12];
    unsigned n = 0;
    do { t[n++] = (char)('0' + v % 10); v /= 10; } while (v && n < sizeof t);
    for (unsigned i = 0; i < n; i++) out[i] = t[n - 1 - i];
    out[n] = 0;
}

typedef struct { const dmt_canvas *canvas; uint32_t last; } screen;
static void boot_progress(void *context, const char *what, uint32_t done, uint32_t total)
{
    screen *s = context;
    const dmt_canvas *c = s->canvas;
    if (!c || (total && done != total && done - s->last < total / 20u)) return;
    s->last = done;
    char line[40];
    unsigned k = 0;
    for (const char *p = what; *p && k < 20; p++) line[k++] = *p;
    line[k++] = ' ';
    if (total) {
        number(line + k, (uint32_t)((uint64_t)done * 100u / total));
        while (line[k]) k++;
        line[k++] = '%';
    } else {
        number(line + k, done);
        while (line[k]) k++;
    }
    line[k] = 0;
    graphics_fill(c, 0, 120, 319, 160, C_BG);
    text(c, 20, 124, 24, C_TEXT, line);
    if (total) {
        graphics_rectangle(c, 20, 152, 299, 159, C_TEXT);
        uint32_t w = (uint32_t)((uint64_t)278 * done / total);
        if (w) graphics_fill(c, 21, 153, (uint16_t)(21 + w - 1), 158, C_ACCENT);
    }
}

uint8_t theme_boot(const dmt_theme_boot *b, uint8_t selected, dmt_theme_active *active, bool *installed,
                   enum dmt_theme_result *result)
{
    FILINFO fi;
    raw_package pkg = {0};
    bool pending = false;
    *installed = false;
    if (filesystem_mount(b->fs, 0) == FR_OK && f_stat("0:/THEME.PKG", &fi) == FR_OK && !(fi.fattrib & AM_DIR) &&
        f_open(b->scratch, "0:/THEME.PKG", FA_READ) == FR_OK) {
        pkg.size = (uint32_t)f_size(b->scratch);
        pkg.start = b->scratch->obj.sclust;
        pkg.fatbase = (uint32_t)b->fs->fatbase;
        pkg.database = (uint32_t)b->fs->database;
        pkg.csize = b->fs->csize;
        pkg.n_fatent = b->fs->n_fatent;
        pending = b->fs->fs_type == FS_FAT12 && pkg.size && pkg.start;
        f_close(b->scratch);
    }
    filesystem_mount(b->fs, 1);
    uint8_t slot = selected;
    if (pending) {
        const dmt_canvas *c = b->canvas;
        screen s = {c, 0};
        dmt_theme_progress progress = {&s, boot_progress};
        dmt_theme_source source = {&pkg, raw_read, pkg.size};
        char name[DMT_THEME_NAME + 1] = {0};
        uint8_t new_slot = 0;
        pkg.cached_cluster = pkg.start;
        if (c) {
            graphics_fill(c, 0, 0, 319, 239, C_BG);
            text(c, 20, 30, 24, C_ACCENT, "THEME.PKG found");
            text(c, 20, 70, 16, C_TEXT, "Do not switch off the meter");
        }
        enum dmt_theme_result r = theme_install(&source, b->scratch, b->buffer, b->buffer_size, &progress, &new_slot, name);
        if (result) *result = r;
        if (c) {
            graphics_fill(c, 0, 100, 319, 239, C_BG);
            text(c, 20, 110, 24, r == DMT_THEME_OK ? C_GOOD : C_BAD, theme_result_text(r));
            if (r == DMT_THEME_OK) text(c, 20, 150, 16, C_TEXT, name[0] ? name : "(unnamed theme)");
            text(c, 20, 190, 16, C_TEXT, "About > Enter: theme list");
        }
        if (r == DMT_THEME_OK) slot = new_slot;
        *installed = true;
        /* Remove the package from the bootloader disk in every case. */
        if (filesystem_mount(b->fs, 0) == FR_OK) (void)f_unlink("0:/THEME.PKG");
        filesystem_mount(b->fs, 1);
        if (b->delay_ms) b->delay_ms(b->context, 2500);
    }
    if (!theme_activate(active, slot, b->scratch)) {
        slot = 0;
        *active = (dmt_theme_active){0};
    }
    return slot;
}
