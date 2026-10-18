#ifndef DMT_THRESHOLDS_H
#define DMT_THRESHOLDS_H
#include <stdbool.h>
#include <stdint.h>
#include "graphics.h"
/* Six sign/magnitude halfwords at 0x20000032..0x2000003c: V low/high,
 * A low/high (hundredths), temperature low/high (Celsius). */
typedef struct {int32_t value,peer;uint8_t index,column;bool active;} dmt_threshold_editor;
int32_t threshold_decode(uint16_t);
uint16_t threshold_encode(int32_t);
bool threshold_editor_begin(dmt_threshold_editor *,const uint16_t words[6],uint8_t menu_id);
bool threshold_editor_step(dmt_threshold_editor *,uint16_t words[6],uint8_t *event);
void threshold_editor_draw(const dmt_threshold_editor *,const dmt_canvas *,uint8_t layout,bool selected);
void thresholds_draw(const uint16_t words[6],const dmt_canvas *,uint8_t layout);
/* 0x13264: 0=inside,1=below,2=above; equality preserves the previous alarm. */
bool threshold_alarm(const uint16_t words[6],uint8_t mode,float value,bool fahrenheit,uint8_t *alarm);
#endif
