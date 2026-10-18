/* Installable theme overlays (see theme.h). Portable: used by the firmware
 * and by the host simulator with the same FatFs volume mapping. */
#include "theme.h"
#include <stddef.h>

enum { HEADER = 48, ENTRY_FIXED = 13, PATH_LIMIT = 64, INF_V1 = 112, INF_SIZE = 176, MAX_FILES = 400, CLUSTER = 4096 };
static const char PKG_MAGIC[8] = {'D', 'M', 'T', '9', '9', 'T', 'H', 'M'};
/* THEME.INF v2 (176 bytes): magic "DMT99TI2", name[24] @8, files u16 @32, palette
 * count u8 @34, bytes u32 @36, bloom[64] @40, package crc u32 @104,
 * palette[16][2] u16 @108, crc32 of bytes 0..171 @172. v1 (0.4.0, 112 bytes,
 * "DMT99TI1", no palette, crc @108) is still read. */
static const char INF_MAGIC[8] = {'D', 'M', 'T', '9', '9', 'T', 'I', '2'};

static uint32_t crc32_update(uint32_t crc, const uint8_t *p, uint32_t n)
{
    static const uint32_t t[16] = {
        0x00000000u, 0x1db71064u, 0x3b6e20c8u, 0x26d930acu, 0x76dc4190u, 0x6b6b51f4u, 0x4db26158u, 0x5005713cu,
        0xedb88320u, 0xf00f9344u, 0xd6d6a3e8u, 0xcb61b38cu, 0x9b64c2b0u, 0x86d3d2d4u, 0xa00ae278u, 0xbdbdf21cu};
    crc = ~crc;
    while (n--) {
        crc ^= *p++;
        crc = (crc >> 4) ^ t[crc & 15];
        crc = (crc >> 4) ^ t[crc & 15];
    }
    return ~crc;
}
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static void wr16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void wr32(uint8_t *p, uint32_t v) { for (unsigned i = 0; i < 4; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static unsigned len(const char *s) { unsigned n = 0; while (s[n]) n++; return n; }

/* Asset path as used for hashing: no drive prefix, lower case. */
static const char *relative(const char *path)
{
    if (path[0] && path[1] == ':' && path[2] == '/') return path + 3;
    return path;
}
static char lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + 32) : c; }

uint32_t theme_hash(const char *path)
{
    uint32_t h = 2166136261u;
    for (const char *p = relative(path); *p; p++) { h ^= (uint8_t)lower(*p); h *= 16777619u; }
    return h;
}
static void bloom_add(uint8_t bloom[64], uint32_t h)
{
    for (unsigned k = 0; k < 3; k++) { unsigned bit = (h >> (9 * k)) & 511u; bloom[bit >> 3] |= (uint8_t)(1u << (bit & 7)); }
}
static bool bloom_has(const uint8_t bloom[64], uint32_t h)
{
    for (unsigned k = 0; k < 3; k++) { unsigned bit = (h >> (9 * k)) & 511u; if (!(bloom[bit >> 3] & (1u << (bit & 7)))) return false; }
    return true;
}

/* "themes/Tnn" (+ "/" + rest) into out; false when it does not fit. */
static bool slot_path(char *out, unsigned n, uint8_t slot, const char *rest)
{
    static const char base[] = "themes/T";
    unsigned k = 0;
    for (const char *p = base; *p; p++) { if (k + 1 >= n) return false; out[k++] = *p; }
    if (k + 3 >= n) return false;
    out[k++] = (char)('0' + slot / 10);
    out[k++] = (char)('0' + slot % 10);
    if (rest) {
        if (k + 1 >= n) return false;
        out[k++] = '/';
        for (const char *p = rest; *p; p++) { if (k + 1 >= n) return false; out[k++] = *p; }
    }
    out[k] = 0;
    return true;
}

bool theme_path(const dmt_theme_active *t, const char *asset, char *out, unsigned n)
{
    if (!t || !t->slot || !asset) return false;
    const char *rel = relative(asset);
    /* 0:/LOGO.jpg is the BOOT volume's own custom logo, never themed. */
    if (asset[0] == '0' && asset[1] == ':') {
        const char *l = "logo.jpg";
        unsigned i = 0;
        while (l[i] && lower(rel[i]) == l[i]) i++;
        if (!l[i] && !rel[i]) return false;
    }
    if (!bloom_has(t->bloom, theme_hash(rel))) return false;
    return slot_path(out, n, t->slot, rel);
}

static bool read_inf(uint8_t slot, FIL *f, uint8_t inf[INF_SIZE])
{
    char path[32];
    UINT got = 0;
    if (!slot || slot > DMT_THEME_SLOTS || !slot_path(path, sizeof path, slot, "THEME.INF")) return false;
    if (f_open(f, path, FA_READ) != FR_OK) return false;
    bool ok = f_read(f, inf, INF_SIZE, &got) == FR_OK;
    f_close(f);
    if (!ok) return false;
    for (unsigned i = 0; i < 7; i++) if (inf[i] != (uint8_t)INF_MAGIC[i]) return false;
    if (inf[7] == '1' && got == INF_V1 && crc32_update(0, inf, INF_V1 - 4) == rd32(inf + INF_V1 - 4)) {
        inf[34] = 0;    /* v1: no palette */
        return true;
    }
    return inf[7] == '2' && got == INF_SIZE && inf[34] <= DMT_THEME_PALETTE &&
           crc32_update(0, inf, INF_SIZE - 4) == rd32(inf + INF_SIZE - 4);
}

bool theme_info(uint8_t slot, dmt_theme_info *out, FIL *f)
{
    uint8_t inf[INF_SIZE];
    *out = (dmt_theme_info){0};
    out->slot = slot;
    if (!read_inf(slot, f, inf)) {
        const char *s = "(incomplete)";
        for (unsigned i = 0; s[i]; i++) out->name[i] = s[i];
        return false;
    }
    out->valid = true;
    for (unsigned i = 0; i < DMT_THEME_NAME; i++) {
        char c = (char)inf[8 + i];
        out->name[i] = c >= 32 && c < 127 ? c : (c ? '?' : 0);
    }
    out->name[DMT_THEME_NAME] = 0;
    out->files = rd16(inf + 32);
    out->bytes = rd32(inf + 36);
    return true;
}

bool theme_activate(dmt_theme_active *t, uint8_t slot, FIL *f)
{
    uint8_t inf[INF_SIZE];
    *t = (dmt_theme_active){0};
    if (!slot) return true;
    if (!read_inf(slot, f, inf)) return false;
    t->slot = slot;
    for (unsigned i = 0; i < 64; i++) t->bloom[i] = inf[40 + i];
    t->palette_count = inf[34];
    for (unsigned i = 0; i < t->palette_count; i++) {
        t->palette[i][0] = rd16(inf + 108 + 4 * i);
        t->palette[i][1] = rd16(inf + 110 + 4 * i);
    }
    return true;
}

uint16_t theme_color(const dmt_theme_active *t, uint16_t c)
{
    for (unsigned i = 0; t && i < t->palette_count; i++)
        if (t->palette[i][0] == c) return t->palette[i][1];
    return c;
}

static int slot_of(const char *name)
{
    if ((name[0] != 'T' && name[0] != 't') || name[1] < '0' || name[1] > '9' || name[2] < '0' || name[2] > '9' || name[3])
        return -1;
    int v = (name[1] - '0') * 10 + (name[2] - '0');
    return v >= 1 && v <= (int)DMT_THEME_SLOTS ? v : -1;
}

unsigned theme_list(uint8_t *slots, unsigned max)
{
    DIR d;
    FILINFO fi;
    unsigned n = 0;
    if (f_opendir(&d, "themes") != FR_OK) return 0;
    while (f_readdir(&d, &fi) == FR_OK && fi.fname[0]) {
        int s = slot_of(fi.fname);
        if (s < 0 || !(fi.fattrib & AM_DIR) || n >= max) continue;
        unsigned k = n++;
        while (k && slots[k - 1] > s) { slots[k] = slots[k - 1]; k--; }
        slots[k] = (uint8_t)s;
    }
    f_closedir(&d);
    return n;
}

uint32_t theme_free_bytes(void)
{
    FATFS *fs;
    DWORD clusters = 0;
    if (f_getfree("1:", &clusters, &fs) != FR_OK) return 0;
    return clusters * (uint32_t)fs->csize * CLUSTER;
}

static void report(const dmt_theme_progress *p, const char *what, uint32_t done, uint32_t total)
{
    if (p && p->progress) p->progress(p->context, what, done, total);
}

enum dmt_theme_result theme_remove(uint8_t slot, const dmt_theme_progress *progress)
{
    char path[112];
    unsigned root;
    uint32_t removed = 0;
    if (!slot || slot > DMT_THEME_SLOTS || !slot_path(path, sizeof path, slot, 0)) return DMT_THEME_E_IO;
    root = len(path);
    for (unsigned guard = 0; guard < 4000; guard++) {
        DIR d;
        FILINFO fi;
        FRESULT r = f_opendir(&d, path);
        if (r != FR_OK) return len(path) == root && (r == FR_NO_PATH || r == FR_NO_FILE) ? DMT_THEME_OK : DMT_THEME_E_IO;
        bool have = f_readdir(&d, &fi) == FR_OK && fi.fname[0];
        f_closedir(&d);
        unsigned n = len(path);
        if (!have) {                                   /* empty folder: remove it, go up */
            if (f_unlink(path) != FR_OK) return DMT_THEME_E_IO;
            if (n == root) {
                (void)f_unlink("themes");   /* only succeeds once the last theme is gone */
                report(progress, "Deleted", removed, removed);
                return DMT_THEME_OK;
            }
            while (n > root && path[n - 1] != '/') n--;
            path[n - 1] = 0;
            continue;
        }
        unsigned k = len(fi.fname);
        if (n + 1 + k + 1 > sizeof path) return DMT_THEME_E_IO;
        path[n] = '/';
        for (unsigned i = 0; i <= k; i++) path[n + 1 + i] = fi.fname[i];
        if (fi.fattrib & AM_DIR) continue;             /* descend */
        if (f_unlink(path) != FR_OK) return DMT_THEME_E_IO;
        path[n] = 0;
        report(progress, "Deleting", ++removed, 0);
    }
    return DMT_THEME_E_IO;
}

static bool src_read(const dmt_theme_source *s, uint32_t offset, uint8_t *out, uint32_t n)
{
    return offset <= s->size && n <= s->size - offset && s->read(s->context, offset, out, n);
}

static bool is_palette(const char *p)
{
    const char *n = "theme.pal";
    unsigned i = 0;
    while (n[i] && lower(p[i]) == n[i]) i++;
    return !n[i] && !p[i];
}

static bool path_ok(const char *p, unsigned n)
{
    unsigned depth = 0;
    if (n < 5 || n > PATH_LIMIT || p[0] == '/' || p[0] == '.') return false;
    for (unsigned i = 0; i < n; i++) {
        char c = p[i];
        bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' ||
                  c == '.' || c == '/' || c == '~';   /* '~': history/~a.jpg, ~v.jpg in the originals */
        if (!ok) return false;
        if (c == '/') { depth++; if (i + 1 < n && (p[i + 1] == '/' || p[i + 1] == '.')) return false; }
    }
    if (depth > 3 || p[n - 1] == '/') return false;
    return lower(p[n - 4]) == '.' && lower(p[n - 3]) == 'j' && lower(p[n - 2]) == 'p' && lower(p[n - 1]) == 'g';
}

typedef struct { uint32_t offset, size, crc; char path[PATH_LIMIT + 1]; uint8_t path_len; } entry;

static bool read_entry(const dmt_theme_source *s, uint32_t *at, entry *e)
{
    uint8_t fixed[ENTRY_FIXED];
    if (!src_read(s, *at, fixed, ENTRY_FIXED)) return false;
    e->offset = rd32(fixed);
    e->size = rd32(fixed + 4);
    e->crc = rd32(fixed + 8);
    e->path_len = fixed[12];
    if (e->path_len > PATH_LIMIT || !src_read(s, *at + ENTRY_FIXED, (uint8_t *)e->path, e->path_len)) return false;
    e->path[e->path_len] = 0;
    *at += ENTRY_FIXED + e->path_len;
    return true;
}

static bool make_dirs(const char *path)
{
    char p[112];
    unsigned n = len(path);
    if (n >= sizeof p) return false;
    for (unsigned i = 0; i <= n; i++) p[i] = path[i];
    for (unsigned i = 0; i < n; i++) {
        if (p[i] != '/') continue;
        p[i] = 0;
        FRESULT r = f_mkdir(p);
        p[i] = '/';
        if (r != FR_OK && r != FR_EXIST) return false;
    }
    return true;
}

enum dmt_theme_result theme_install(const dmt_theme_source *s, FIL *f, uint8_t *buf, uint32_t bufsize,
                                    const dmt_theme_progress *progress, uint8_t *slot_out,
                                    char name_out[DMT_THEME_NAME + 1])
{
    uint8_t h[HEADER];
    if (bufsize < 512 || !src_read(s, 0, h, HEADER)) return DMT_THEME_E_FORMAT;
    for (unsigned i = 0; i < 8; i++) if (h[i] != (uint8_t)PKG_MAGIC[i]) return DMT_THEME_E_FORMAT;
    uint16_t count = rd16(h + 10);
    uint32_t total = rd32(h + 12), dir = rd32(h + 40), data = rd32(h + 44);
    if (rd16(h + 8) != 1 || !count || count > MAX_FILES || total != s->size || total < HEADER + 4 || dir != HEADER ||
        data < dir || data > total - 4)
        return DMT_THEME_E_FORMAT;
    for (unsigned i = 0; i < DMT_THEME_NAME; i++) {
        char c = (char)h[16 + i];
        name_out[i] = c >= 32 && c < 127 ? c : (c ? '?' : 0);
    }
    name_out[DMT_THEME_NAME] = 0;

    /* 1. Whole-package CRC. */
    uint32_t crc = 0;
    for (uint32_t at = 0; at < total - 4;) {
        uint32_t n = total - 4 - at < bufsize ? total - 4 - at : bufsize;
        if (!src_read(s, at, buf, n)) return DMT_THEME_E_IO;
        crc = crc32_update(crc, buf, n);
        at += n;
        report(progress, "Checking", at, total);
    }
    uint8_t tail[4];
    if (!src_read(s, total - 4, tail, 4)) return DMT_THEME_E_IO;
    if (crc != rd32(tail)) return DMT_THEME_E_CRC;
    uint32_t package_crc = crc;

    /* 2. Directory: paths replace existing original JPEGs, data in range;
     * an optional THEME.PAL entry carries the colour palette. */
    uint8_t bloom[64] = {0}, palette[4 * DMT_THEME_PALETTE];
    uint8_t palette_count = 0;
    uint16_t images = 0;
    uint32_t clusters = 16, bytes = 0, at = dir;
    for (uint16_t i = 0; i < count; i++) {
        entry e;
        FILINFO fi;
        uint8_t head[3];
        if (!read_entry(s, &at, &e) || at > data) return DMT_THEME_E_FORMAT;
        if (is_palette(e.path)) {
            if (palette_count || !e.size || e.size % 4 || e.size > sizeof palette || e.offset < data ||
                e.offset > total - 4 || e.size > total - 4 - e.offset || !src_read(s, e.offset, palette, e.size) ||
                crc32_update(0, palette, e.size) != e.crc)
                return DMT_THEME_E_FORMAT;
            palette_count = (uint8_t)(e.size / 4);
            continue;
        }
        if (!path_ok(e.path, e.path_len)) return DMT_THEME_E_PATH;
        if (e.size < 4 || e.offset < data || e.offset > total - 4 || e.size > total - 4 - e.offset) return DMT_THEME_E_FORMAT;
        if (!src_read(s, e.offset, head, 3) || head[0] != 0xff || head[1] != 0xd8 || head[2] != 0xff) return DMT_THEME_E_PATH;
        if (f_stat(e.path, &fi) != FR_OK || (fi.fattrib & AM_DIR)) return DMT_THEME_E_PATH;
        bloom_add(bloom, theme_hash(e.path));
        clusters += (e.size + CLUSTER - 1) / CLUSTER;
        bytes += e.size;
        images++;
    }
    if (!images && !palette_count) return DMT_THEME_E_FORMAT;
    if (theme_free_bytes() / CLUSTER < clusters) return DMT_THEME_E_SPACE;

    /* 3. First free slot. */
    char path[112];
    uint8_t slot = 0;
    for (uint8_t k = 1; k <= DMT_THEME_SLOTS && !slot; k++) {
        FILINFO fi;
        if (!slot_path(path, sizeof path, k, 0)) return DMT_THEME_E_IO;
        FRESULT st = f_stat(path, &fi);
        if (st == FR_NO_FILE || st == FR_NO_PATH) slot = k;   /* NO_PATH: no themes folder yet */
    }
    if (!slot) return DMT_THEME_E_FULL;
    FRESULT r = f_mkdir("themes");
    if (r != FR_OK && r != FR_EXIST) return DMT_THEME_E_IO;
    if (f_mkdir(path) != FR_OK) return DMT_THEME_E_IO;

    /* 4. Copy and verify each file. */
    enum dmt_theme_result result = DMT_THEME_OK;
    at = dir;
    for (uint16_t i = 0; i < count && result == DMT_THEME_OK; i++) {
        entry e;
        UINT done;
        if (!read_entry(s, &at, &e)) { result = DMT_THEME_E_IO; break; }
        if (is_palette(e.path)) continue;
        if (!slot_path(path, sizeof path, slot, e.path) || !make_dirs(path)) { result = DMT_THEME_E_IO; break; }
        if (f_open(f, path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) { result = DMT_THEME_E_IO; break; }
        uint32_t file_crc = 0;
        for (uint32_t off = 0; off < e.size && result == DMT_THEME_OK;) {
            uint32_t n = e.size - off < bufsize ? e.size - off : bufsize;
            if (!src_read(s, e.offset + off, buf, n)) result = DMT_THEME_E_IO;
            else {
                file_crc = crc32_update(file_crc, buf, n);
                if (f_write(f, buf, n, &done) != FR_OK || done != n) result = DMT_THEME_E_IO;
            }
            off += n;
        }
        if (f_close(f) != FR_OK && result == DMT_THEME_OK) result = DMT_THEME_E_IO;
        if (result != DMT_THEME_OK) break;
        if (file_crc != e.crc) { result = DMT_THEME_E_CRC; break; }
        if (f_open(f, path, FA_READ) != FR_OK) { result = DMT_THEME_E_VERIFY; break; }
        uint32_t back = 0, total_read = 0;
        for (;;) {
            if (f_read(f, buf, bufsize, &done) != FR_OK) { result = DMT_THEME_E_VERIFY; break; }
            if (!done) break;
            back = crc32_update(back, buf, done);
            total_read += done;
        }
        f_close(f);
        if (result == DMT_THEME_OK && (back != e.crc || total_read != e.size)) result = DMT_THEME_E_VERIFY;
        report(progress, "Installing", i + 1u, count);
    }

    /* 5. THEME.INF last: only now is the slot valid. */
    if (result == DMT_THEME_OK) {
        uint8_t inf[INF_SIZE] = {0};
        UINT done;
        for (unsigned i = 0; i < 8; i++) inf[i] = (uint8_t)INF_MAGIC[i];
        for (unsigned i = 0; i < DMT_THEME_NAME; i++) inf[8 + i] = (uint8_t)name_out[i];
        wr16(inf + 32, images);
        inf[34] = palette_count;
        wr32(inf + 36, bytes);
        for (unsigned i = 0; i < 64; i++) inf[40 + i] = bloom[i];
        wr32(inf + 104, package_crc);
        for (unsigned i = 0; i < 4u * palette_count; i++) inf[108 + i] = palette[i];
        wr32(inf + INF_SIZE - 4, crc32_update(0, inf, INF_SIZE - 4));
        if (!slot_path(path, sizeof path, slot, "THEME.INF") || f_open(f, path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
            result = DMT_THEME_E_IO;
        else {
            if (f_write(f, inf, INF_SIZE, &done) != FR_OK || done != INF_SIZE) result = DMT_THEME_E_IO;
            if (f_close(f) != FR_OK) result = DMT_THEME_E_IO;
        }
    }
    if (result != DMT_THEME_OK) { (void)theme_remove(slot, 0); return result; }
    *slot_out = slot;
    return DMT_THEME_OK;
}

const char *theme_result_text(enum dmt_theme_result r)
{
    switch (r) {
    case DMT_THEME_OK: return "Installed";
    case DMT_THEME_E_FORMAT: return "Not a valid theme package";
    case DMT_THEME_E_CRC: return "Theme package is damaged";
    case DMT_THEME_E_PATH: return "Package contains unknown files";
    case DMT_THEME_E_SPACE: return "Not enough space: delete a theme";
    case DMT_THEME_E_FULL: return "No free theme slot";
    case DMT_THEME_E_IO: return "Flash write error";
    case DMT_THEME_E_VERIFY: return "Verify failed";
    }
    return "Error";
}

uint8_t theme_settings_slot(const uint16_t *h)
{
    uint16_t slot = h[DMT_THEME_SETTINGS_INDEX];
    return slot >= 1 && slot <= DMT_THEME_SLOTS && h[DMT_THEME_SETTINGS_INDEX + 1] == (slot ^ 0xa5a5u) ? (uint8_t)slot : 0;
}
void theme_settings_store(uint16_t *h, uint8_t slot)
{
    h[DMT_THEME_SETTINGS_INDEX] = slot;
    h[DMT_THEME_SETTINGS_INDEX + 1] = (uint16_t)(slot ^ 0xa5a5u);
}
