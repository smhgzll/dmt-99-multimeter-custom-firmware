#ifndef DMT_CONTROL_H
#define DMT_CONTROL_H
#include <stdbool.h>
#include <stdint.h>
#include "measurement_asic.h"
/* Semantic state; comments identify original globals, not an SRAM overlay. */
typedef struct {
    uint8_t view,mode,unit,layout; /* 0x200001de,206,289,205 */
    uint8_t event,menu_last;     /* 0x200001ee,1da */
    uint8_t regular_selector[4];/* 0x200001fc..1ff */
    uint8_t record_selector[3]; /* 0x20000200..202 */
    uint8_t monitor_selector,monitor_state; /* 203,204 */
    uint8_t relative,relative_mode,relative_unit; /* 1f4..1f6 */
    uint8_t ac_dc_code,status_high; /* 0x20000288,28a */
    uint8_t hold,numeric;       /* 23e,27c */
    float value,relative_value; /* 284,1f8 */
    char text[16];              /* 0x2000028c (bounded semantic buffer) */
} dmt_control;
enum dmt_effect {
    DMT_EFFECT_RELATIVE_ON, DMT_EFFECT_RELATIVE_OFF, DMT_EFFECT_BEEP,
    DMT_EFFECT_MENU_OPEN, DMT_EFFECT_HISTORY_APPEND, DMT_EFFECT_BUZZER_OFF,
    DMT_EFFECT_HOLD_ON, DMT_EFFECT_HOLD_OFF, DMT_EFFECT_MODE_CHANGED,
    DMT_EFFECT_MONITOR_CONFIG, DMT_EFFECT_SCREEN_RESTORE, DMT_EFFECT_READING, DMT_EFFECT_AUTO_READING
};
typedef struct {
    void *context;
    void (*send)(void *,const uint8_t command[10]);
    bool (*longpress)(void *,uint8_t event); /* 0x7b08: synchronous input */
    void (*effect)(void *,enum dmt_effect,const dmt_control *);
} dmt_control_io;
void control_buttons(dmt_control *,const dmt_control_io *);
void control_after_menu(dmt_control *); /* 0xb5ae plus caller-specific reset */
void control_event(dmt_control *,const uint8_t frame[13],const dmt_control_io *);
bool control_parse(dmt_control *,const uint8_t frame[13]);
float measurement_parse_number(const char *); /* 0x9774 -> 0x96de */
float measurement_relative_value(const dmt_control *);
#endif
