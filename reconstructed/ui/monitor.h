#ifndef DMT_MONITOR_H
#define DMT_MONITOR_H
#include "view.h"
void monitor_alarm_render(const dmt_view *,const uint16_t[6],uint8_t,float,bool,uint8_t,uint8_t *);
void compact_unit_text(char out[4],uint8_t unit,bool fahrenheit); /*12b00 glyph codes*/
void compact_unit_draw(const dmt_view *,uint8_t,uint16_t,uint16_t,bool);
void monitor_threshold_render(const dmt_view *,const uint16_t[6],uint8_t mode,uint8_t layout,bool english,bool fahrenheit);
#endif
