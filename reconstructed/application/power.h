#ifndef DMT_POWER_H
#define DMT_POWER_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {uint8_t ticks,battery_due;uint16_t idle_seconds,threshold_seconds;bool shutdown;} dmt_power;
typedef struct {
    void *context;
    int (*battery_percent)(void *);bool (*charging)(void *);
    void (*low_battery_countdown)(void *); /* 0x14c42: 3,2,1 images + delay */
    void (*power_down)(void *);          /* 0xc320 */
} dmt_power_io;
void power_timer_update(dmt_power *,uint16_t settings,const dmt_power_io *);
#endif
