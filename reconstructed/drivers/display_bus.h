#ifndef DMT_DISPLAY_BUS_H
#define DMT_DISPLAY_BUS_H
#include "dmt_bus.h"
void display_write_command(const dmt_display_bus *bus,uint16_t value);
void display_write_data(const dmt_display_bus *bus,uint16_t value);
#endif
