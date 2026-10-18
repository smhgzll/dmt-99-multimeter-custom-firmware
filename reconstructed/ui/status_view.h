#ifndef DMT_STATUS_VIEW_H
#define DMT_STATUS_VIEW_H
#include "graphics.h"
typedef struct {void *context;bool (*image)(void *,const char *,uint16_t,uint16_t);void (*delay_ms)(void *,uint32_t);const dmt_canvas *canvas;} dmt_status_view;
void status_battery(const dmt_status_view *,int percentage,uint8_t layout);
void status_unit(const dmt_status_view *,uint16_t x,uint16_t y,uint8_t unit,uint8_t *last_unit);
void status_live(const dmt_status_view *,bool active,uint8_t *last);
void status_ncv(const dmt_status_view *,uint8_t level,uint8_t layout);
void status_charging(const dmt_status_view *,const char *language_prefix);
void status_low_battery_countdown(const dmt_status_view *,const char *language_prefix,uint8_t language);
void status_language_choice(const dmt_status_view *,uint8_t choice,bool initial,bool confirmed);
void status_scrollbar(const dmt_status_view *,uint8_t position,uint8_t layout);
void status_about_version(const dmt_status_view *);
#endif
