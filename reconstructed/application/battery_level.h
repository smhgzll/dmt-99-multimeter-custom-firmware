#ifndef DMT_BATTERY_LEVEL_H
#define DMT_BATTERY_LEVEL_H
#include <stdint.h>
/* 75f0 arithmetic after the 50-sample ADC mean (channel 10, PC0):
 * mV=(raw*6600)>>12, level=1-(4000-mV)/600, result=(int)(level*100).
 * Single-precision, truncating, unclamped (may be negative or above 100). */
int battery_level_percent(uint16_t raw,float *millivolts,float *level);
#endif
