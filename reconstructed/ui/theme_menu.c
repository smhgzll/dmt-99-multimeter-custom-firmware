/* "Display & themes" screen, opened with ENTER on the About page (release
 * builds). First the simple-screen settings (on/off, colours, digit size),
 * then the theme list (original + installed themes).
 * Keys follow the menu: 1/2 move; on setting rows 5 (OK) next value and
 * 3 (ENTER) previous value; on theme rows 5 uses the theme and 3 deletes it
 * (asks first, OK confirms); 4 BACK / 6 MENU leave.
 * Drawn with the firmware font only, so no artwork is needed. */
#include "theme_ui.h"

#define C_BG 0x0000u
#define C_TEXT 0xffffu
#define C_DIM 0x8410u
#define C_BAR 0x1c9fu
#define C_ROW 0x2945u
#define C_WARN 0xfd40u
enum { ROWS = 7, ROW_H = 22, LIST_Y = 34, MSG_Y = 194 };

static void put(const dmt_canvas *c, uint16_t x, uint16_t y, uint8_t size, uint16_t fg, uint16_t bg, const char *s)
{
    graphics_ascii_text(c, x, y, s, size, fg, bg, false);
}
static unsigned copy(char *out, unsigned at, unsigned cap, const char *s)
{
    while (*s && at + 1 < cap) out[at++] = *s++;
    out[at] = 0;
    return at;
}
static unsigned decimal(char *out, unsigned at, unsigned cap, uint32_t v)
{
    char t[12];
    unsigned n = 0;
    do { t[n++] = (char)('0' + v % 10); v /= 10; } while (v);
    while (n && at + 1 < cap) out[at++] = t[--n];
    out[at] = 0;
    return at;
}
static unsigned length(const char *s) { unsigned n = 0; while (s[n]) n++; return n; }
static void size_text(char *out, unsigned cap, uint32_t bytes)
{
    unsigned k = 0;
    if (bytes >= 1048576u) {
        k = decimal(out, k, cap, bytes / 1048576u);
        k = copy(out, k, cap, ".");
        k = decimal(out, k, cap, (bytes % 1048576u) * 10u / 1048576u);
        copy(out, k, cap, " MB");
    } else {
        k = decimal(out, k, cap, (bytes + 1023u) / 1024u);
        copy(out, k, cap, " KB");
    }
}

static void message(const dmt_canvas *c, uint16_t color, const char *s)
{
    graphics_fill(c, 0, MSG_Y, 319, MSG_Y + 17, C_BG);
    if (s) put(c, 8, MSG_Y, 16, color, C_BG, s);
}
static void footer(const dmt_theme_menu *m, const dmt_canvas *c)
{
    graphics_fill(c, 0, 214, 319, 239, C_ROW);
    put(c, 4, 219, 12, C_TEXT, C_ROW, m->cursor < SIMPLE_FIELDS ? "OK: next   ENTER: previous   BACK/MENU: exit"
                                                                : "OK: use   ENTER: delete   BACK/MENU: exit");
}

static void row(const dmt_theme_menu *m, const dmt_theme_io *io, const dmt_canvas *c, unsigned index)
{
    if (index < m->top || index >= (unsigned)m->top + ROWS) return;
    uint16_t y = (uint16_t)(LIST_Y + (index - m->top) * ROW_H);
    bool here = index == m->cursor;
    uint16_t bg = here ? C_BAR : C_BG;
    graphics_fill(c, 0, y, 319, (uint16_t)(y + ROW_H - 2), bg);
    if (index >= m->count) return;
    if (index < SIMPLE_FIELDS) {                         /* simple-screen setting */
        dmt_simple_config cfg;
        io->simple_get(io->context, &cfg);
        enum dmt_simple_field f = (enum dmt_simple_field)index;
        put(c, 4, (uint16_t)(y + 2), 16, C_TEXT, bg, simple_field_name(f));
        const char *v = simple_field_value(&cfg, f);
        uint16_t x = (uint16_t)(300 - 8 * length(v));
        put(c, x, (uint16_t)(y + 2), 16, here ? C_TEXT : C_WARN, bg, v);
        if (f >= SIMPLE_BACKGROUND && f <= SIMPLE_ACCENT) {
            uint8_t ci = f == SIMPLE_BACKGROUND ? cfg.background : f == SIMPLE_DIGITS ? cfg.digits : f == SIMPLE_TEXT ? cfg.text : cfg.accent;
            graphics_fill(c, 304, (uint16_t)(y + 3), 316, (uint16_t)(y + ROW_H - 5), simple_color(ci));
            graphics_rectangle(c, 303, (uint16_t)(y + 2), 317, (uint16_t)(y + ROW_H - 4), C_DIM);
        }
        return;
    }
    uint8_t slot = m->slots[index - SIMPLE_FIELDS];
    char name[DMT_THEME_NAME + 4], size[16];
    dmt_theme_info info = {0};
    bool valid = slot == 0 || io->info(io->context, slot, &info);
    unsigned k = copy(name, 0, sizeof name, slot == io->active(io->context) ? "* " : "  ");
    copy(name, k, sizeof name, slot ? info.name : "Original (FNIRSI)");
    if (slot && !name[2]) copy(name, 2, sizeof name, "(unnamed)");
    put(c, 4, (uint16_t)(y + 2), 16, valid ? C_TEXT : C_WARN, bg, name);
    if (slot && valid) size_text(size, sizeof size, info.bytes);
    else copy(size, 0, sizeof size, slot ? "broken" : "built-in");
    put(c, (uint16_t)(316 - 8 * length(size)), (uint16_t)(y + 2), 16, here ? C_TEXT : C_DIM, bg, size);
}

static void list(const dmt_theme_menu *m, const dmt_theme_io *io, const dmt_canvas *c)
{
    for (unsigned i = m->top; i < (unsigned)m->top + ROWS; i++) row(m, io, c, i);
}

static void frame(const dmt_theme_menu *m, const dmt_theme_io *io, const dmt_canvas *c)
{
    char free_text[24], size[16];
    graphics_fill(c, 0, 0, 319, 239, C_BG);
    graphics_fill(c, 0, 0, 319, 27, C_BAR);
    put(c, 8, 2, 24, C_TEXT, C_BAR, "Display");
    size_text(size, sizeof size, io->free_bytes(io->context));
    unsigned k = copy(free_text, 0, sizeof free_text, "free ");
    copy(free_text, k, sizeof free_text, size);
    put(c, 200, 7, 16, C_TEXT, C_BAR, free_text);
    footer(m, c);
    list(m, io, c);
}

static void load(dmt_theme_menu *m, const dmt_theme_io *io)
{
    m->slots[0] = 0;
    m->count = (uint8_t)(SIMPLE_FIELDS + 1 + io->list(io->context, m->slots + 1, DMT_THEME_LIST_MAX));
    if (m->cursor >= m->count) m->cursor = (uint8_t)(m->count - 1);
    if (m->cursor < m->top) m->top = m->cursor;
    if (m->cursor >= m->top + ROWS) m->top = (uint8_t)(m->cursor - ROWS + 1);
}

void theme_menu_open(dmt_theme_menu *m, const dmt_theme_io *io, const dmt_canvas *c)
{
    *m = (dmt_theme_menu){0};
    load(m, io);
    frame(m, io, c);
}

typedef struct { const dmt_canvas *c; } progress_ctx;
static void delete_progress(void *context, const char *what, uint32_t done, uint32_t total)
{
    progress_ctx *p = context;
    char line[40];
    unsigned k = copy(line, 0, sizeof line, what);
    k = copy(line, k, sizeof line, " ");
    decimal(line, k, sizeof line, done);
    (void)total;
    message(p->c, C_TEXT, line);
}

bool theme_menu_step(dmt_theme_menu *m, uint8_t *event, const dmt_theme_io *io, const dmt_canvas *c)
{
    uint8_t e = *event;
    if (!e) return false;
    *event = 0;
    if (m->confirm) {
        uint8_t slot = m->slots[m->cursor - SIMPLE_FIELDS];
        m->confirm = false;
        if (e != 5) { message(c, C_DIM, "Delete cancelled"); return false; }
        progress_ctx p = {c};
        dmt_theme_progress progress = {&p, delete_progress};
        bool was_active = io->active(io->context) == slot;
        enum dmt_theme_result r = io->remove(io->context, slot, &progress);
        if (was_active) io->select(io->context, 0);
        load(m, io);
        frame(m, io, c);
        message(c, r == DMT_THEME_OK ? C_TEXT : C_WARN, r == DMT_THEME_OK ? "Theme deleted" : theme_result_text(r));
        return false;
    }
    if (e == 1 || e == 2) {
        uint8_t old = m->cursor, old_top = m->top;
        if (e == 1) m->cursor = (uint8_t)(m->cursor + 1 < m->count ? m->cursor + 1 : 0);
        else m->cursor = (uint8_t)(m->cursor ? m->cursor - 1 : m->count - 1);
        load(m, io);
        if (m->top != old_top) list(m, io, c);
        else { row(m, io, c, old); row(m, io, c, m->cursor); }
        if ((old < SIMPLE_FIELDS) != (m->cursor < SIMPLE_FIELDS)) footer(m, c);
        message(c, C_TEXT, 0);
        return false;
    }
    if (e == 4 || e == 6) return true;
    if (m->cursor < SIMPLE_FIELDS) {                     /* setting rows */
        if (e != 5 && e != 3) return false;
        dmt_simple_config cfg;
        io->simple_get(io->context, &cfg);
        simple_field_step(&cfg, (enum dmt_simple_field)m->cursor, e == 5 ? 1 : -1);
        io->simple_set(io->context, &cfg);
        row(m, io, c, m->cursor);
        if (m->cursor == SIMPLE_ON)
            message(c, C_TEXT, cfg.on ? "Simple screen on (Regular view)" : "Simple screen off");
        return false;
    }
    uint8_t slot = m->slots[m->cursor - SIMPLE_FIELDS];
    if (e == 5) {
        dmt_theme_info info;
        if (slot && !io->info(io->context, slot, &info)) { message(c, C_WARN, "Broken theme: delete it"); return false; }
        uint8_t old = io->active(io->context);
        if (!io->select(io->context, slot)) { message(c, C_WARN, "Cannot activate theme"); return false; }
        for (uint8_t i = SIMPLE_FIELDS; i < m->count; i++) if (m->slots[i - SIMPLE_FIELDS] == old) row(m, io, c, i);
        row(m, io, c, m->cursor);
        message(c, C_TEXT, slot ? "Theme active" : "Original theme active");
        return false;
    }
    if (e == 3) {
        if (!slot) { message(c, C_WARN, "The original cannot be deleted"); return false; }
        m->confirm = true;
        message(c, C_WARN, "Delete? OK: yes  BACK: no");
    }
    return false;
}

void theme_about_hint(const dmt_canvas *c)
{
    put(c, 166, 196, 16, 0x8410u, 0x0000u, "ENTER: display");
}
