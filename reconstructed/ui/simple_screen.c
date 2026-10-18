/* Image-free "simple screen" for the Regular view (see simple_screen.h).
 *
 * Layout (320x240):
 *   y   0..33  mode name (24 px font) left, battery right
 *   y  44..124 value, classic digit glyphs scaled 1.0 / 1.3 / 1.55 (sign cell narrow)
 *   y 156..196 unit (32 px font, custom glyphs for Ohm, micro, degree)
 *   y 204..239 AC/DC, HOLD, REL, AUTO
 * Only changed characters are repainted on each reading. */
#include "simple_screen.h"
#include "bitmap_images.h"
#include "format.h"
#include "measurement_asic.h"
#include "reading.h"
#include "view.h"
#include "settings.h"

static const uint16_t COLORS[DMT_SIMPLE_COLORS] = {
    0x0000, 0xffff, 0x8410, 0x39e7, 0xf800, 0xfd40, 0xffe0, 0x87e0,
    0x07e0, 0x0320, 0x07ff, 0x1c9f, 0x0010, 0x801f, 0xf81f};
static const char *const COLOR_NAMES[DMT_SIMPLE_COLORS] = {
    "Black", "White", "Grey", "Dark grey", "Red", "Orange", "Yellow", "Lime",
    "Green", "Dark green", "Cyan", "Blue", "Navy", "Purple", "Pink"};
static const char *const SIZE_NAMES[3] = {"Small", "Medium", "Large"};
static const uint16_t SCALE[3] = {256, 333, 396};          /* Q8: 1.0, 1.3, 1.55 */
enum { INDEX = 110, GW = 38, GH = 51, VALUE_Y = 44, UNIT_Y = 160, BAR_Y = 206 };
enum { F_HOLD = 1, F_REL = 2, F_AC = 4, F_DC = 8, F_AUTO = 16 };

/* ------------------------------------------------------------- settings */
void simple_config_load(const uint16_t *h, dmt_simple_config *c)
{
    uint16_t a = h[INDEX], b = h[INDEX + 1], d = h[INDEX + 2];
    bool ok = (a & 0xff00) == 0xa500 && h[INDEX + 3] == (uint16_t)(a ^ b ^ d ^ 0x5a5a) &&
              (b & 0xff) < DMT_SIMPLE_COLORS && (b >> 8) < DMT_SIMPLE_COLORS &&
              (d & 0xff) < DMT_SIMPLE_COLORS && (d >> 8) < DMT_SIMPLE_COLORS && ((a >> 1) & 3) < 3;
    if (!ok) {   /* defaults: off, large orange digits on black, white text, blue accent */
        *c = (dmt_simple_config){false, 2, 0, 5, 1, 11};
        return;
    }
    *c = (dmt_simple_config){(a & 1) != 0, (uint8_t)((a >> 1) & 3), (uint8_t)(b & 0xff), (uint8_t)(b >> 8),
                             (uint8_t)(d & 0xff), (uint8_t)(d >> 8)};
}
void simple_config_store(uint16_t *h, const dmt_simple_config *c)
{
    h[INDEX] = (uint16_t)(0xa500 | (c->on ? 1 : 0) | (c->size << 1));
    h[INDEX + 1] = (uint16_t)(c->background | c->digits << 8);
    h[INDEX + 2] = (uint16_t)(c->text | c->accent << 8);
    h[INDEX + 3] = (uint16_t)(h[INDEX] ^ h[INDEX + 1] ^ h[INDEX + 2] ^ 0x5a5a);
}
uint16_t simple_color(uint8_t i) { return COLORS[i < DMT_SIMPLE_COLORS ? i : 0]; }
const char *simple_color_name(uint8_t i) { return COLOR_NAMES[i < DMT_SIMPLE_COLORS ? i : 0]; }
const char *simple_field_name(enum dmt_simple_field f)
{
    static const char *const names[SIMPLE_FIELDS] = {"Simple screen", "Background", "Digits", "Text", "Accent", "Digit size"};
    return f < SIMPLE_FIELDS ? names[f] : "";
}
const char *simple_field_value(const dmt_simple_config *c, enum dmt_simple_field f)
{
    switch (f) {
    case SIMPLE_ON: return c->on ? "On" : "Off";
    case SIMPLE_BACKGROUND: return simple_color_name(c->background);
    case SIMPLE_DIGITS: return simple_color_name(c->digits);
    case SIMPLE_TEXT: return simple_color_name(c->text);
    case SIMPLE_ACCENT: return simple_color_name(c->accent);
    case SIMPLE_SIZE: return SIZE_NAMES[c->size < 3 ? c->size : 2];
    default: return "";
    }
}
static uint8_t cycle(uint8_t v, unsigned n, int d) { return (uint8_t)((v + n + (d < 0 ? -1 : 1)) % n); }
void simple_field_step(dmt_simple_config *c, enum dmt_simple_field f, int d)
{
    switch (f) {
    case SIMPLE_ON: c->on = !c->on; break;
    case SIMPLE_BACKGROUND: c->background = cycle(c->background, DMT_SIMPLE_COLORS, d); break;
    case SIMPLE_DIGITS: c->digits = cycle(c->digits, DMT_SIMPLE_COLORS, d); break;
    case SIMPLE_TEXT: c->text = cycle(c->text, DMT_SIMPLE_COLORS, d); break;
    case SIMPLE_ACCENT: c->accent = cycle(c->accent, DMT_SIMPLE_COLORS, d); break;
    case SIMPLE_SIZE: c->size = cycle(c->size, 3, d); break;
    default: break;
    }
}

/* -------------------------------------------------------------- drawing */
/* 8x16 glyphs the ROM font lacks: Ohm, micro, degree (scaled x2 for 32 px). */
static const uint8_t OHM[16] = {0, 0, 0, 0x3c, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0xe7, 0, 0, 0};
static const uint8_t MICRO[16] = {0, 0, 0, 0, 0, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x7a, 0x40, 0x40, 0x80, 0};
static const uint8_t DEGREE[16] = {0, 0, 0x30, 0x48, 0x48, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void glyph8(const dmt_canvas *c, uint16_t x, uint16_t y, const uint8_t *bits, unsigned scale, uint16_t fg, uint16_t bg)
{
    for (unsigned r = 0; r < 16; r++)
        for (unsigned col = 0; col < 8; col++)
            graphics_fill(c, (uint16_t)(x + col * scale), (uint16_t)(y + r * scale), (uint16_t)(x + col * scale + scale - 1),
                          (uint16_t)(y + r * scale + scale - 1), (bits[r] & (0x80 >> col)) ? fg : bg);
}
/* Text in the ROM font; '~' = Ohm, '`' = micro, '^' = degree. Returns the end x. */
static uint16_t text(const dmt_canvas *c, uint16_t x, uint16_t y, uint8_t size, uint16_t fg, uint16_t bg, const char *s)
{
    unsigned scale = size >= 32 ? 2 : 1;
    for (; *s; s++) {
        const uint8_t *special = *s == '~' ? OHM : *s == '`' ? MICRO : *s == '^' ? DEGREE : 0;
        if (special && size != 24) glyph8(c, x, y, special, scale, fg, bg);
        else graphics_ascii(c, x, y, (uint8_t)(special ? '?' : *s), size, fg, bg, false);
        x = (uint16_t)(x + size / 2);
    }
    return x;
}
static uint16_t text_width(uint8_t size, const char *s)
{
    uint16_t n = 0;
    while (s[n]) n++;
    return (uint16_t)(n * (size / 2));
}

static const char *mode_name(uint8_t mode)
{
    switch (mode) {
    case 0x09: return "NCV";
    case 0x0a: return "CAP";
    case 0x0b: return "RES";
    case 0x0c: return "DC V";
    case 0x0d: return "AC V";
    case 0x0e: return "CONT";
    case 0x0f: return "FREQ";
    case 0x10: return "DC A";
    case 0x11: return "DC mA";
    case 0x12: return "TEMP";
    case 0x13: return "LIVE";
    case 0x14: return "AUTO";
    case 0x15: return "AC A";
    case 0x16: return "AC mA";
    default: return "";
    }
}
static const char *unit_name(uint8_t unit, bool fahrenheit)
{
    switch (unit) {
    case 6: return "V";
    case 7: return "mV";
    case 8: return "`A";
    case 9: return "mA";
    case 10: return "A";
    case 11: return "~";
    case 12: return "k~";
    case 13: return "M~";
    case 15: return "nF";
    case 16: return "`F";
    case 17: return "mF";
    case 18: return "Hz";
    case 19: return "kHz";
    case 20: return "MHz";
    case 21: return fahrenheit ? "^F" : "^C";
    default: return "";
    }
}

static uint16_t cell_w(uint8_t size) { return (uint16_t)((GW * SCALE[size] + 255) / 256 + 6); }
static uint16_t cell_h(uint8_t size) { return (uint16_t)((GH * SCALE[size] + 255) / 256); }
static uint16_t dot_w(uint8_t size) { return (uint16_t)(cell_w(size) / 3); }

/* Width of character i of a value text: '.' narrow, a leading sign/blank half. */
static uint16_t char_w(uint8_t s, const char *t, unsigned i)
{
    if (t[i] == '.') return dot_w(s);
    if (i == 0 && t[0] == ' ') return 0;                  /* blank sign position: not reserved */
    if (i == 0 && t[0] == '-') return (uint16_t)(cell_w(s) / 2);
    return cell_w(s);
}

/* One value character into its cell (digits, A L T U O, '-', '.', blank). */
static void value_char(const dmt_canvas *c, const dmt_simple_config *cfg, uint16_t x, uint16_t w, char ch)
{
    uint8_t s = cfg->size < 3 ? cfg->size : 2;
    uint16_t h = cell_h(s), fg = simple_color(cfg->digits), bg = simple_color(cfg->background);
    graphics_fill(c, x, VALUE_Y, (uint16_t)(x + w - 1), (uint16_t)(VALUE_Y + h - 1), bg);
    if (ch == '.') {
        uint16_t d = (uint16_t)(h / 9);
        graphics_fill(c, (uint16_t)(x + w / 2 - d / 2), (uint16_t)(VALUE_Y + h - d - 1), (uint16_t)(x + w / 2 + d / 2),
                      (uint16_t)(VALUE_Y + h - 1), fg);
        return;
    }
    if (ch == '-') {
        uint16_t t = (uint16_t)(h / 10 + 1);
        graphics_fill(c, (uint16_t)(x + 3), (uint16_t)(VALUE_Y + h / 2 - t / 2), (uint16_t)(x + w - 4),
                      (uint16_t)(VALUE_Y + h / 2 + t / 2), fg);
        return;
    }
    int g = ch >= '0' && ch <= '9' ? ch - '0' : ch == 'O' ? 0 : ch == 'A' ? 10 : ch == 'L' ? 11 : ch == 'T' ? 12 : ch == 'U' ? 13 : -1;
    if (g < 0) {
        if (ch != ' ' && ch) graphics_ascii(c, (uint16_t)(x + (w > 16 ? (w - 16) / 2 : 0)), (uint16_t)(VALUE_Y + (h - 32) / 2),
                                            (uint8_t)ch, 32, fg, bg, false);
        return;
    }
    const uint8_t *p = numeric_bitmap_glyphs[g] + 6;
    unsigned stride = (GW + 7) / 8, scale = SCALE[s], gw = w > 6 ? w - 6u : 0;
    for (unsigned dy = 0; dy < h; dy++) {
        unsigned sy = dy * 256 / scale;
        if (sy >= GH) break;
        unsigned run = 0;
        bool on = false;
        for (unsigned dx = 0; dx <= gw; dx++) {
            unsigned sx = dx * 256 / scale;
            bool bit = dx < gw && sx < GW && (p[sy * stride + sx / 8] & (0x80 >> (sx % 8)));
            if (bit && !on) { run = dx; on = true; }
            if (!bit && on) {
                graphics_fill(c, (uint16_t)(x + 3 + run), (uint16_t)(VALUE_Y + dy), (uint16_t)(x + 3 + dx - 1), (uint16_t)(VALUE_Y + dy), fg);
                on = false;
            }
        }
    }
}

/* Positions: up to 7 characters; the block is centred. */
static void layout(const dmt_simple_config *cfg, const char *t, uint16_t xs[8], uint16_t ws[8], uint16_t *total)
{
    uint8_t s = cfg->size < 3 ? cfg->size : 2;
    uint16_t w = 0;
    for (unsigned i = 0; i < 7 && t[i]; i++) w = (uint16_t)(w + char_w(s, t, i));
    uint16_t x = w < 320 ? (uint16_t)((320 - w) / 2) : 0;
    for (unsigned i = 0; i < 8; i++) {
        xs[i] = x;
        ws[i] = i < 7 && t[i] ? char_w(s, t, i) : 0;
        x = (uint16_t)(x + ws[i]);
    }
    *total = w;
}

static void value(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, const char *t)
{
    char now[8] = {0};
    for (unsigned i = 0; i < 7 && t[i]; i++) now[i] = t[i];
    uint16_t xs[8], xo[8], ws[8], wo[8], wn, wt;
    layout(cfg, now, xs, ws, &wn);
    layout(cfg, k->text, xo, wo, &wt);
    bool same_layout = k->valid && wn == wt;
    for (unsigned i = 0; i < 8 && same_layout; i++) if (xs[i] != xo[i] || ws[i] != wo[i]) same_layout = false;
    if (!same_layout) {
        uint8_t s = cfg->size < 3 ? cfg->size : 2;
        graphics_fill(c, 0, VALUE_Y, 319, (uint16_t)(VALUE_Y + cell_h(s) - 1), simple_color(cfg->background));
    }
    for (unsigned i = 0; i < 7 && now[i]; i++)
        if (ws[i] && (!same_layout || now[i] != k->text[i])) value_char(c, cfg, xs[i], ws[i], now[i]);
    for (unsigned i = 0; i < 8; i++) k->text[i] = now[i];
}

static void ncv_live(const dmt_canvas *c, const dmt_simple_config *cfg, const dmt_control *s, const uint8_t f[13])
{
    uint16_t bg = simple_color(cfg->background), fg = simple_color(cfg->digits);
    graphics_fill(c, 0, VALUE_Y, 319, UNIT_Y + 39, bg);
    if (s->mode == 9) {
        unsigned level = 0;
        for (unsigned i = 0; i < 4; i++) level += measurement_segment_decode((uint8_t)((f[2 + i] & 0xf0) | (f[3 + i] & 15))) == 16;
        for (unsigned i = 0; i < 4; i++)
            graphics_fill(c, (uint16_t)(40 + i * 62), (uint16_t)(140 - i * 25), (uint16_t)(90 + i * 62), 150,
                          i < level ? fg : simple_color(cfg->text == cfg->background ? 2 : 3));
    } else {
        bool live = measurement_segment_decode((uint8_t)((f[2] & 0xf0) | (f[3] & 15))) != 14;
        const char *msg = live ? "LIVE" : "---";
        text(c, (uint16_t)(160 - text_width(32, msg) / 2), 90, 32, live ? simple_color(cfg->accent) : fg, bg, msg);
    }
}

static void unit(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, uint8_t u, bool f)
{
    uint16_t bg = simple_color(cfg->background);
    if (k->valid && k->unit == u) return;
    graphics_fill(c, 0, UNIT_Y, 319, UNIT_Y + 39, bg);
    const char *name = unit_name(u, f);
    text(c, (uint16_t)(160 - text_width(32, name) / 2), (uint16_t)(UNIT_Y + 4), 32, simple_color(cfg->text), bg, name);
    k->unit = u;
}

static void tag(const dmt_canvas *c, const dmt_simple_config *cfg, uint16_t x, const char *s, bool on)
{
    uint16_t bg = simple_color(cfg->background);
    uint16_t w = (uint16_t)(text_width(24, s) + 10);
    if (on) {
        graphics_fill(c, x, BAR_Y + 4, (uint16_t)(x + w), BAR_Y + 31, simple_color(cfg->accent));
        text(c, (uint16_t)(x + 5), BAR_Y + 6, 24, bg, simple_color(cfg->accent), s);
    } else {
        graphics_fill(c, x, BAR_Y + 4, (uint16_t)(x + w), BAR_Y + 31, bg);
    }
}

static uint8_t flags_of(const dmt_control *s)
{
    /* AC/DC only where the meter measures voltage or current (incl. AUTO). */
    bool vi = s->mode == 0x0c || s->mode == 0x0d || s->mode == 0x10 || s->mode == 0x11 || s->mode == 0x14 ||
              s->mode == 0x15 || s->mode == 0x16;
    return (uint8_t)((s->hold ? F_HOLD : 0) | (s->relative ? F_REL : 0) | (vi && s->ac_dc_code == 1 ? F_AC : 0) |
                     (vi && s->ac_dc_code == 2 ? F_DC : 0) | (s->mode == 0x14 ? F_AUTO : 0));
}
static void bar(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, const dmt_control *s)
{
    uint8_t f = flags_of(s);
    if (k->valid && k->flags == f) return;
    tag(c, cfg, 4, (f & F_AC) ? "AC" : "DC", (f & (F_AC | F_DC)) != 0);
    tag(c, cfg, 70, "HOLD", f & F_HOLD);
    tag(c, cfg, 160, "REL", f & F_REL);
    tag(c, cfg, 236, "AUTO", f & F_AUTO);
    k->flags = f;
}

static void top(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, uint8_t mode)
{
    uint16_t bg = simple_color(cfg->background);
    if (k->valid && k->mode == mode) return;
    graphics_fill(c, 0, 0, 200, 33, bg);
    text(c, 8, 5, 24, simple_color(cfg->text), bg, mode_name(mode));
    graphics_fill(c, 0, 36, 319, 37, simple_color(cfg->accent));
    k->mode = mode;
}

void simple_battery(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, uint8_t p)
{
    uint16_t bg = simple_color(cfg->background), fg = simple_color(cfg->text);
    char t[6];
    unsigned n = 0;
    if (p > 100) p = 100;
    if (p >= 100) t[n++] = '1';
    if (p >= 10) t[n++] = (char)('0' + (p / 10) % 10);
    t[n++] = (char)('0' + p % 10);
    t[n++] = '%';
    t[n] = 0;
    graphics_fill(c, 204, 0, 319, 33, bg);
    text(c, (uint16_t)(268 - text_width(16, t)), 10, 16, fg, bg, t);
    graphics_rectangle(c, 274, 9, 310, 25, fg);
    graphics_fill(c, 311, 13, 313, 21, fg);
    uint16_t level = (uint16_t)(p * 33 / 100);
    if (level) graphics_fill(c, 276, 11, (uint16_t)(276 + level - 1), 23, p < 15 ? 0xf800 : simple_color(cfg->accent));
    k->battery = p;
}

void simple_base(const dmt_canvas *c, dmt_screen *scr, const dmt_simple_config *cfg, dmt_simple_cache *k,
                 const dmt_control *s, const uint8_t frame[13], uint16_t settings, uint8_t battery, bool splash)
{
    if (splash && scr && !scr->splash_seen) {   /* same splash as screen_initial */
        scr->splash_seen = true;
        if (!scr->logo_failed || scr->logo_failed(scr->context)) view_image(&scr->view, "0:/on.jpg", 0, 0);
        if (scr->backlight) scr->backlight(scr->context, settings_backlight_compare(settings));
        if (scr->delay_ms) scr->delay_ms(scr->context, 1000);
    }
    if (scr) scr->normal = true;
    graphics_fill(c, 0, 0, 319, 239, simple_color(cfg->background));
    *k = (dmt_simple_cache){0};
    top(c, cfg, k, s->mode);
    simple_battery(c, cfg, k, battery);
    k->valid = 1;
    k->unit = 0xff;
    k->flags = 0xff;
    bar(c, cfg, k, s);
    (void)frame;
}

void simple_reading(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, const dmt_control *cs,
                    const uint8_t frame[13], uint16_t settings)
{
    dmt_control *s = (dmt_control *)cs;   /* updates s->text exactly as reading_render does for view 1 */
    top(c, cfg, k, s->mode);
    if (s->mode == 9 || s->mode == 0x13) {
        ncv_live(c, cfg, s, frame);
        k->text[0] = 0;
        k->unit = 0xff;
        bar(c, cfg, k, s);
        return;
    }
    reading_decode_text(s->text, frame, s->mode);
    if (s->numeric) {
        float shown = s->value;
        if (s->relative == 1) shown -= s->relative_value;
        if (s->mode == 0x12 && ((settings >> 5) & 1)) shown = (float)((double)shown * 1.8 + 32.0);
        measurement_format_value(s->text, sizeof s->text, shown, s->mode, s->unit);
    }
    value(c, cfg, k, s->text);
    unit(c, cfg, k, s->unit, (settings & 32) != 0);
    bar(c, cfg, k, s);
}

void simple_event(const dmt_canvas *c, const dmt_simple_config *cfg, dmt_simple_cache *k, const dmt_control *s)
{
    if (k->valid && k->mode != s->mode) {      /* new mode: clear value and unit */
        uint8_t sz = cfg->size < 3 ? cfg->size : 2;
        graphics_fill(c, 0, VALUE_Y, 319, (uint16_t)(UNIT_Y + 39 > VALUE_Y + cell_h(sz) ? UNIT_Y + 39 : VALUE_Y + cell_h(sz)),
                      simple_color(cfg->background));
        k->text[0] = 0;
        for (unsigned i = 0; i < 8; i++) k->text[i] = 0;
        k->unit = 0xff;
    }
    top(c, cfg, k, s->mode);
    bar(c, cfg, k, s);
}
