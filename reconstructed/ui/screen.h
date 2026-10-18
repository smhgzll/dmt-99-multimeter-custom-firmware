#ifndef DMT_SCREEN_H
#define DMT_SCREEN_H
#include "reading.h"
#include "menu.h"
#include "history.h"
typedef struct {
 dmt_view view;
 dmt_reading reading;
 bool splash_seen,normal;
 uint8_t seen_view,unit_cache,auto_cache,history_unit;
 uint16_t history_unit_y;
 void *context;
 bool (*logo_failed)(void *);
 void (*delay_ms)(void *,uint32_t);
 void (*backlight)(void *,uint8_t);
 /* Optional debug console: a910 prints "LCD_Fill" (no newline) when HOLD
  * ends in theme 2. NULL discards it. */
 void (*debug)(void *,const char *);
} dmt_screen;
/* Power-on values: reading_power_on plus the unit cache (shared with the
 * menu cache at 0x200001da, initial 0x19) and AUTO label cache (0x200001d9,
 * initial 0xff). Other members start at zero as in the original BSS. */
void screen_power_on(dmt_screen *);
void screen_base(dmt_screen *,dmt_control *,const uint8_t[13],uint16_t,uint8_t);
void screen_initial(dmt_screen *,dmt_control *,const uint8_t[13],uint16_t,uint8_t);
void screen_menu(dmt_screen *,const dmt_menu *,const dmt_control *,uint16_t,const uint16_t[6],const dmt_history *);
void screen_event(dmt_screen *,const dmt_control *,uint16_t,enum dmt_effect,bool hold_toggle);
void screen_reading(dmt_screen *,dmt_control *,const uint8_t[13],uint16_t);
#endif
