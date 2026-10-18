#ifndef DMT_SIMPLE_SCREEN_H
#define DMT_SIMPLE_SCREEN_H
/* "Simple screen": an image-free measurement screen for the Regular view
 * (release builds). Drawn only with fills, the ROM font and the classic digit
 * glyphs; background, digit, text and accent colours and the digit size are
 * chosen on the meter (About > ENTER) and kept in settings halfwords 110..113. */
#include "control.h"
#include "graphics.h"
#include "screen.h"
#include <stdbool.h>
#include <stdint.h>

#define DMT_SIMPLE_COLORS 15u
enum dmt_simple_field { SIMPLE_ON, SIMPLE_BACKGROUND, SIMPLE_DIGITS, SIMPLE_TEXT, SIMPLE_ACCENT, SIMPLE_SIZE, SIMPLE_FIELDS };

typedef struct {
    bool on;
    uint8_t size;                 /* 0 small, 1 medium, 2 large */
    uint8_t background, digits, text, accent;   /* colour indexes */
} dmt_simple_config;

typedef struct {                  /* what is on the panel, to redraw only changes */
    char text[8];
    uint8_t unit, mode, flags, battery, valid;
} dmt_simple_cache;

void simple_config_load(const uint16_t *settings_halfwords, dmt_simple_config *);
void simple_config_store(uint16_t *settings_halfwords, const dmt_simple_config *);
uint16_t simple_color(uint8_t index);
const char *simple_color_name(uint8_t index);
/* Value text of one menu field ("On", "Lime", "Large"). */
const char *simple_field_value(const dmt_simple_config *, enum dmt_simple_field);
const char *simple_field_name(enum dmt_simple_field);
void simple_field_step(dmt_simple_config *, enum dmt_simple_field, int direction);

/* Full repaint (base); `screen` supplies the splash on first use. */
void simple_base(const dmt_canvas *, dmt_screen *, const dmt_simple_config *, dmt_simple_cache *,
                 const dmt_control *, const uint8_t frame[13], uint16_t settings, uint8_t battery, bool splash);
void simple_reading(const dmt_canvas *, const dmt_simple_config *, dmt_simple_cache *, const dmt_control *,
                    const uint8_t frame[13], uint16_t settings);
void simple_event(const dmt_canvas *, const dmt_simple_config *, dmt_simple_cache *, const dmt_control *);
void simple_battery(const dmt_canvas *, const dmt_simple_config *, dmt_simple_cache *, uint8_t percent);
#endif
