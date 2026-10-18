#ifndef DMT_MODES_H
#define DMT_MODES_H
#include <stdint.h>
/* 0xa910 ties these event IDs to specific UI resource paths. */
enum dmt_measurement_mode {
    DMT_MODE_CAPACITANCE=0x0a,DMT_MODE_RESISTANCE=0x0b,
    DMT_MODE_DC_V=0x0c,DMT_MODE_AC_V=0x0d,DMT_MODE_0E_OL_ICON=0x0e,
    DMT_MODE_FREQUENCY=0x0f,DMT_MODE_DC_A=0x10,DMT_MODE_DC_MA=0x11,
    DMT_MODE_TEMPERATURE=0x12,DMT_MODE_14_DEFAULT=0x14,
    DMT_MODE_AC_A=0x15,DMT_MODE_AC_MA=0x16
};
#endif
