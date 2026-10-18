#ifndef DMT_GRAPHICS_H
#define DMT_GRAPHICS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct {
    void *context;
    void (*pixel)(void *,uint16_t x,uint16_t y,uint16_t color);
    uint16_t width,height;
    /* Optional panel fast path for graphics_fill (original 0x8a02 window and
     * row stream). NULL draws the same pixels one at a time. */
    void (*fill)(void *,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color);
} dmt_canvas;
void graphics_line(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void graphics_rectangle(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void graphics_fill(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void graphics_cut_fill(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t);
void graphics_cut_border(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t);
void graphics_ascii(const dmt_canvas *,uint16_t,uint16_t,uint8_t,uint8_t,uint16_t,uint16_t,bool);
void graphics_measurement_char(const dmt_canvas *,uint16_t,uint16_t,uint8_t,uint8_t,uint16_t,bool);
void graphics_ascii_text(const dmt_canvas *,uint16_t,uint16_t,const char *,uint8_t,uint16_t,uint16_t,bool);
void graphics_text_box(const dmt_canvas *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t,const char *,bool special_width);
void graphics_number(const dmt_canvas *,uint16_t,uint16_t,uint32_t,uint8_t,uint8_t,uint8_t,uint16_t);
void graphics_bitmap(const dmt_canvas *,uint16_t,uint16_t,const uint8_t *,size_t,uint16_t,uint16_t);
/* Preserved ROM bytes, original addresses 0x1edbc..0x21dbb. */
extern const uint8_t dmt_font_rom[12288];
#endif
