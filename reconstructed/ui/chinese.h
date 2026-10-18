#ifndef DMT_CHINESE_H
#define DMT_CHINESE_H
#include "display.h"
/* Two-byte character codes as stored in firmware (first byte in high8 bits).
 * Opaque mode deliberately retains the original window/data sequence; it is
 * not normalized to a guessed panel protocol. */
void display_chinese_glyph(const dmt_display *,uint16_t x,uint16_t y,uint16_t code,
                          uint16_t foreground,uint16_t background,uint8_t size,bool transparent);
void display_chinese_string(const dmt_display *,uint16_t x,uint16_t y,const uint8_t *text,
                           uint16_t foreground,uint16_t background,uint8_t size,bool transparent);
extern const uint8_t dmt_chinese_rom[6214]; /* 0x21dbc..0x23601 */
#endif
