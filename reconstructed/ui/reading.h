#ifndef DMT_READING_H
#define DMT_READING_H
#include "control.h"
#include "graph.h"
#include "view.h"
typedef struct {
 float maximum,minimum;
 int32_t maximum_unit,minimum_unit;
 uint8_t previous_mode;
 /* Original9caa resistance branch reads a local stack byte/string before
  * initialization. Explicit inputs preserve the observed CPU-state contract. */
 uint8_t resistance_guard;
 char scratch_unit[4];
 dmt_graph graph;
} dmt_reading;
void extrema_format(char *,size_t,float); /*13504*/
void reading_reset_extrema(dmt_reading *); /*96c0*/
/* Initialized SRAM of the V2.4 image, 0x200001b0..0x200001c3: maximum
 * -9999, minimum 9999, units -9999/9999 and previous mode 0xff. */
void reading_power_on(dmt_reading *);
void reading_decode_text(char out[16],const uint8_t frame[13],uint8_t mode);
void reading_render(dmt_reading *,dmt_control *,const uint8_t frame[13],uint16_t settings,const dmt_view *);
#endif
