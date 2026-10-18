#ifndef DMT_FW_RUNTIME_H
#define DMT_FW_RUNTIME_H
/* State shared between firmware/main.c (foreground) and firmware/isr.c. */
#include "application.h"
#include "config.h"
#include "buttons.h"

/* USART1 IDLE snapshot. The ISR applies the original c468 bookkeeping to this
 * shadow (latest frame, blank suppression, latest FD event); the foreground
 * moves it into the application with interrupts masked, so a frame is never
 * parsed while it is being replaced. */
typedef struct {
    uint8_t frame[13],saved_event[13];
    uint8_t frame_ready,rx_suppressed,event_pending;
    uint32_t frames,events;
} fw_rx_shadow;

typedef struct {
    uint32_t loops,tx_sent,tx_rejected,unresolved,last_unresolved;
    uint32_t timer_ticks_lost,emergency_off;
} fw_counters_t;

extern dmt_application fw_app;
extern uint8_t fw_dma_frame[13];
extern volatile fw_rx_shadow fw_rx;
extern volatile uint8_t fw_timer_pending;      /* TIM3 updates not yet serviced */
extern volatile uint8_t fw_shutdown_request;   /* PC2 power key in normal operation */
extern volatile uint8_t fw_shutdown_age;       /* seconds a request has waited */
#if DMT_THEMES
extern volatile uint8_t fw_theme_busy;         /* theme install/delete in progress */
#endif
extern fw_counters_t fw_counters;
extern const dmt_button_io fw_button_io;
extern const dmt_settings_backend fw_settings_backend; /* internal_flash.c: d928/d87c */
#if DMT_LAST_MODE
extern const dmt_last_mode_backend fw_last_mode_backend; /* last_mode_flash.c: settings halfwords 120..127 */
#endif
bool fw_normal_operation(void);                /* original 0x20000251 */
void fw_emergency_power_off(void) __attribute__((noreturn));
#endif
