#ifndef DMT_GRAPH_H
#define DMT_GRAPH_H
#include "graphics.h"
#include "display.h"
/* SRAM3320 precedes the196-float bank at3324. Original index0 writes it;
 * model that write explicitly instead of invoking C array underflow. */
typedef struct {
 float leading_sample,samples[196];
 uint16_t index,count;
 uint8_t strip;
 double scale;
 uint16_t pixels[196*20]; /* column-major strip at original3634 */
} dmt_graph;
void graph_strip_pixel(dmt_graph *,uint32_t x,uint32_t y,uint16_t color);
void graph_strip_line(dmt_graph *,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void graph_strip_blit(const dmt_graph *,const dmt_canvas *,const dmt_display *,uint16_t,uint16_t);
/* Original17cce ignores value arguments and reads the canonical global value.
 * a1==1 resets; s5 supplies an inherited raw float maximum seed. Neither is
 * silently corrected. The caller must make that register input explicit. */
void graph_update(dmt_graph *,float canonical,uint8_t mode,uint8_t unit,
                  uint32_t a1,uint32_t inherited_s5,const dmt_canvas *,const dmt_display *);
#endif
