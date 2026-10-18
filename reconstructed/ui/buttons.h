#ifndef DMT_BUTTONS_H
#define DMT_BUTTONS_H
#include <stdbool.h>
#include <stdint.h>
enum { BUTTON_PC10=1, BUTTON_PC11=2, BUTTON_PC9=3, BUTTON_PC12=4,
       BUTTON_PC8_SHORT=5, BUTTON_PC8_LONG=6, BUTTON_PC2=8 };
typedef struct {
    void *context;
    bool (*pin_high)(void *,uint16_t mask);
    void (*delay_ms)(void *,uint16_t milliseconds);
    void (*clear_exti)(void *,uint16_t mask);
    void (*power_down)(void *);
} dmt_button_io;
void buttons_exti_2(const dmt_button_io *,uint8_t *event,bool normal_operation);
void buttons_exti_9_5(const dmt_button_io *,uint16_t pending,uint8_t *event,uint16_t *idle_counter);
void buttons_exti_15_10(const dmt_button_io *,uint16_t pending,uint8_t *event,uint16_t *idle_counter);
bool buttons_wait_longpress(const dmt_button_io *,uint8_t key,uint8_t *event);
#endif
