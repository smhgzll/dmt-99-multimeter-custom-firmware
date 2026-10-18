#ifndef DMT_FOREGROUND_H
#define DMT_FOREGROUND_H
#include <stdint.h>
#include <stdbool.h>
/* Semantic state, not a binary overlay of the original globals. */
typedef struct {
    uint8_t view;                 /* original 0x200001de */
    uint8_t mode;                 /* 0x20000206 */
    uint8_t layout;               /* 0x20000205 */
    uint8_t hold;                 /* 0x2000023e */
    uint8_t rx_suppressed;        /* 0x200001c4 */
    uint8_t event_consumed;       /* 0x200001c6 */
    uint8_t last_ui_event;        /* 0x200001d9 */
    uint8_t battery_due;          /* 0x200001ec */
    uint8_t battery_percent;      /* 0x20000248 */
    uint8_t frame[16];            /* 0x200002ac; DMA fills 13 */
    uint8_t saved_event[16];      /* 0x200002bc; ISR copies 13 */
} dmt_foreground_state;
typedef struct {
    void *context;
    void (*regular_buttons)(void *); /* 0xb5c8 */
    void (*record_buttons)(void *);  /* 0xb9e0 */
    void (*monitor_buttons)(void *); /* 0xbc90 */
    void (*parse_measurement)(void *,const uint8_t *); /* 0xbe78 */
    void (*regular_event)(void *);   /* 0xa910 */
    void (*record_event)(void *);    /* 0xaed0 */
    void (*monitor_event)(void *);   /* 0xb29e */
    void (*charge_screen_poll)(void *); /* 0x14cd6; may not return */
    uint8_t (*battery_read)(void *); /* 0x75f0; original caller stores low byte */
    void (*battery_layout_0)(void *); /* 0x14e48 */
    void (*battery_layout_1)(void *); /* 0x14d62 */
} dmt_foreground_ops;
void foreground_iteration(dmt_foreground_state *state,const dmt_foreground_ops *ops);
/* Recovered diagnostic branch of8ed4, one pass through its inner polling loop.
 * Returning false is a host yield; the original immediately polls again. */
typedef struct {
    void *context;
    void (*forward_button)(void *);                 /* 97f8 */
    void (*parse)(void *,const uint8_t *);           /* be78 */
    const char *(*text)(void *);                    /* be24 */
    void (*draw_text)(void *,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t,const char *); /* 8672 */
    void (*draw_unit)(void *,uint16_t,uint16_t);     /* c11a */
} dmt_diagnostic_ops;
bool foreground_diagnostic_iteration(dmt_foreground_state *,const dmt_diagnostic_ops *);
typedef struct {
    void *context;
    bool (*pin_high)(void *,uint16_t); /* GPIOC, short-circuit PC10/PC11 */
    void (*backlight)(void *,uint16_t,uint16_t,uint16_t); /* 7f66 */
    void (*delay_ms)(void *,uint32_t);
    void (*diagnostic_entry)(void *); /* b56a */
    void (*load_settings)(void *);    /* c248; may update state.view */
    void (*initial_screen)(void *);   /* 1511a */
} dmt_foreground_entry_ops;
bool foreground_enter(dmt_foreground_state *,uint8_t *button_event,uint8_t *normal_operation,const dmt_foreground_entry_ops *);
/* Full nonreturning8ed4 call/state reference. Offline main never invokes it.
 * Callees are explicit; separately qualified helpers own their implementations. */
void foreground_loop(dmt_foreground_state *,uint8_t *,uint8_t *,const dmt_foreground_entry_ops *,const dmt_foreground_ops *,const dmt_diagnostic_ops *);
#endif
