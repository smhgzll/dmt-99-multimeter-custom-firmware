#ifndef DMT_MEASUREMENT_ASIC_H
#define DMT_MEASUREMENT_ASIC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "buttons.h"

enum { MEASUREMENT_RX_BYTES = 13, MEASUREMENT_TX_BYTES = 10 };
typedef enum {
    MEASUREMENT_INVALID, MEASUREMENT_DISPLAY_FRAME, MEASUREMENT_EVENT_FRAME
} measurement_frame_kind;
typedef struct {
    measurement_frame_kind kind;
    uint8_t raw[MEASUREMENT_RX_BYTES];
    uint8_t symbol_codes[4];
    char symbols[4];
    uint8_t decimal_after; /* 1..3, or 0 for no decimal marker */
    uint8_t ac_dc_code; /* recovered values 1, 2, 255; interpretation not forced */
    uint8_t unit_code; /* original firmware's numeric unit ID */
    bool negative;
    bool high_status_bit;
    uint8_t event;
} measurement_frame;
typedef struct {
    uint8_t saved_event[MEASUREMENT_RX_BYTES];
    bool suppress_measurement;
    bool event_pending;
} measurement_rx_state;
typedef struct {
    uint8_t mode,numeric,unit,ac_dc_code,status_high;
    float value;
    char text[16];
} measurement_reading;

uint8_t measurement_segment_decode(uint8_t segments);
bool measurement_build_command(uint8_t command, uint8_t out[MEASUREMENT_TX_BYTES]);
/* 97f8 diagnostic/forwarding path. GPIO debounce and UART are explicit calls;
 * this does not assign any analog/ASIC meaning to command IDs. */
void measurement_forward_button(uint8_t *event,const dmt_button_io *,void *tx_context,void (*send)(void *,const uint8_t[10]));
/* B56A exact original void-call contract, independent of inferred semantics. */
void measurement_diagnostic_entry(void *,void (*send)(void *,const uint8_t[10]),void (*delay_ms)(void *,uint32_t));
bool measurement_parse_reading(measurement_reading *,const uint8_t[13]);
float measurement_parse_number(const char *);
bool measurement_frame_is_blank(const uint8_t *frame);
void measurement_capture_idle(measurement_rx_state *state, const uint8_t *frame);
measurement_frame_kind measurement_decode_frame(const uint8_t *data, size_t length, measurement_frame *out);
#endif
