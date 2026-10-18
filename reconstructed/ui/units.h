#ifndef DMT_UNITS_H
#define DMT_UNITS_H
#include "view.h"
/* Shared original unit cache 200001da. Called after the a7a4 dispatcher. */
void units_regular(const dmt_view *,uint8_t,uint8_t,uint8_t,bool,uint8_t *);
void units_recording(const dmt_view *,uint8_t,uint8_t,uint8_t,bool,uint8_t *);
void units_monitor(const dmt_view *,uint8_t,uint8_t,uint8_t,bool,uint8_t *);
#endif
