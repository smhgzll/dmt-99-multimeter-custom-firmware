#ifndef DMT_WCH_RUNTIME_H
#define DMT_WCH_RUNTIME_H
#include <stdbool.h>
#include <stdint.h>
/* SDK peripheral set-up routines shared by the target firmware. No routine
 * here is called by host tests; they touch MMIO directly. */
void recovered_buttons_init(void (*delay_ms)(void *,uint32_t),void *);   /* 7944 */
void recovered_periodic_timer_init(uint16_t period,uint16_t prescaler);  /* 7736 */
void recovered_display_gpio_init(void (*delay_ms)(void *,uint32_t),void *); /* 87aa GPIO/reset */
void recovered_display_set_pc(void *,uint16_t,bool);
void recovered_display_write_pb(void *,uint16_t);
void recovered_backlight_pwm(uint16_t period,uint16_t prescaler,uint16_t compare); /* 7f66 */
void recovered_backlight_compare(uint8_t);                               /* 7f66(100,4799,c) */
void recovered_buzzer_enable(bool);                                      /* 78d0 */
#endif
