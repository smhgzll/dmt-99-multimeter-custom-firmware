#ifndef DMT_GAUGE_H
#define DMT_GAUGE_H
#include "view.h"
void gauge_bitmap(const dmt_view *,const uint8_t *,size_t,uint32_t original_rom,uint16_t,uint16_t,uint16_t,uint16_t,float);
const uint8_t *scene_data(uint32_t original_address,size_t length);
const uint8_t *scene_bitmap(uint32_t,size_t *);
void gauge_lower(const dmt_view *,unsigned);
void gauge_upper(const dmt_view *,unsigned);
void gauge_legacy(const dmt_view *,uint32_t);
#endif
