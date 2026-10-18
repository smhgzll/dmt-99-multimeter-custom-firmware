#ifndef DMT_DISPLAY_H
#define DMT_DISPLAY_H
#include "display_bus.h"
#include "graphics.h"
typedef struct {dmt_display_bus bus;uint16_t width,height;uint8_t column_command,row_command,pixels_command,orientation;} dmt_display;
void display_orientation(dmt_display *,bool landscape);
void display_scan_direction(const dmt_display *,uint32_t direction); /* 7e68; dimensions already selected */
void display_cursor(const dmt_display *,uint16_t,uint16_t);
void display_window(const dmt_display *,uint16_t,uint16_t,uint16_t,uint16_t);
void display_pixel(void *,uint16_t,uint16_t,uint16_t);
void display_solid(const dmt_display *,uint16_t);
void display_rectangle_fill(const dmt_display *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void display_panel_sequence(dmt_display *,void (*delay_ms)(void *,uint32_t),void *);
#endif
