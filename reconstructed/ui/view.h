#ifndef DMT_VIEW_H
#define DMT_VIEW_H
#include "status_view.h"
#include <stddef.h>
typedef struct {
 const dmt_status_view *status;
 void *context;
 /* Optional semantic call trace, used by original-code differential tests. */
 void (*trace)(void *,uint32_t address,const uint32_t args[8],const char *text);
 void (*buzzer)(void *,bool);
 /* 13264 consumes application-owned threshold/alarm globals. The caller binds
  * those through this explicit call interface; trace-only tests may isolate it. */
 void (*monitor_alarm)(void *,uint8_t mode,float value,bool fahrenheit,uint8_t layout);
} dmt_view;
void view_trace(const dmt_view *,uint32_t,const uint32_t[8],const char *);
void view_image(const dmt_view *,const char *,uint16_t,uint16_t);
void view_fill(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void view_text(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t,const char *);
void view_chinese(const dmt_view *,uint16_t,uint16_t,const uint8_t *,uint8_t,uint16_t);
void view_numeric(const dmt_view *,const char *,uint8_t,uint8_t,uint8_t,uint8_t);
void view_ascii(const dmt_view *,uint16_t,uint16_t,const char *,uint16_t,uint16_t,uint8_t);
void view_line(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void view_border(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void view_pixel(const dmt_view *,uint16_t,uint16_t,uint16_t);
void view_measurement_char(const dmt_view *,uint16_t,uint16_t,uint8_t,uint8_t,uint16_t);
void view_cut_fill(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t);
void view_cut_border(const dmt_view *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t);
void view_ascii_char(const dmt_view *,uint16_t,uint16_t,uint8_t,uint8_t,uint16_t);
void view_bitmap(const dmt_view *,uint32_t rom,uint16_t,uint16_t,uint16_t,uint16_t);
void view_glyph(const dmt_view *,uint16_t,uint16_t,uint32_t rom,uint8_t,uint16_t);
#endif
