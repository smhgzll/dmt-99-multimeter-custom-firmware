#ifndef DMT99_ASIC_H
#define DMT99_ASIC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* No MMIO, UART I/O, allocation, calibration or target-programming operations.
 * Wire observations: AUTO display 5AA5/12 bytes, event candidate AA55FD/10.
 * Display semantics and command templates are recovered-host interpretations;
 * analog value/unit/range behavior remains subject to physical validation.
 * RX checksum remains UNKNOWN for every accepted frame.
 */
#define DMT99_MAX_RX_BYTES 13u
#define DMT99_TX_BYTES 10u
typedef enum { DMT99_RX_UNKNOWN, DMT99_RX_DISPLAY, DMT99_RX_EVENT } dmt99_rx_kind;
typedef struct {
    uint8_t raw[DMT99_MAX_RX_BYTES], length;
    dmt99_rx_kind kind;
    uint8_t symbols[4], decimal_after, unit_code, ac_dc_code, event_id;
    /* ac_dc_code: V, mA and A open-probe observations correlate 1 with AC, 2 with DC.
     * 255 means neither low flag set. Do not infer analog accuracy or other
     * untested mode semantics from this code alone. */
    bool negative, status_b7_bit7, numeric;
    int32_t mantissa; /* Four decimal digits with sign, only when numeric. */
    uint16_t divisor; /* 1,10,100,1000; value is in the reported display unit. */
    char text[8];
} dmt99_rx;

/* Explicit complete-frame input; caller keeps ownership of all original data.
 * Accepts observed 12/10-byte forms and a separately chosen historical
 * 13-byte 5AA5 form. Acceptance does not authenticate integrity or ACK semantics.
 * For n<=13, raw input is retained in out even when the return value is false.
 */
bool dmt99_parse_rx(const uint8_t *data, size_t n, dmt99_rx *out);
uint8_t dmt99_decode_segment(uint8_t value);
const char *dmt99_unit_resource_name(uint8_t code);
/* Exact pattern observed with open probes in resistance mode. This does not
 * distinguish every possible overload/fault state or alter the original text. */
bool dmt99_is_resistance_open_pattern(const dmt99_rx *);
/* Recovered screen.c UI states; host_mode must be supplied by the caller.
 * 09: number of '-' segment codes (0..4), 13: first symbol != L (0/1).
 * No inference of host mode, field strength or energized-wire safety.
 * Open-input forms observed; active-input thresholds remain unvalidated.
 * Unsupported mode/input returns false and leaves output unchanged. */
bool dmt99_host_ui_status(const dmt99_rx *, uint8_t host_mode, uint8_t *out);
typedef enum { DMT99_QUANTITY_UNKNOWN, DMT99_QUANTITY_VOLT, DMT99_QUANTITY_AMPERE,
    DMT99_QUANTITY_OHM, DMT99_QUANTITY_FARAD, DMT99_QUANTITY_HERTZ } dmt99_quantity;
/* Reported unit conversion from recovered UI associations, not calibration.
 * Unknown/ambiguous temperature units and nonnumeric/event frames fail;
 * both outputs remain unchanged on failure. */
bool dmt99_value_si(const dmt99_rx *, double *value, dmt99_quantity *quantity);
/* Display-unit conversion, not calibration/nominal component correction.
 * Units 11/12/13 use recovered ohm/kohm/Mohm associations. Requires numeric.
 * Output is unchanged on failure, including AUTO, open pattern and blank data.
 */
bool dmt99_resistance_ohms(const dmt99_rx *, double *out);
/* Literal-only serializer: known ordinary templates plus legacy 00/check11.
 * No arbitrary payload checksum and no diagnostic A0 builder. No transmission.
 */
bool dmt99_build_firmware_template(uint8_t command_id, uint8_t out[DMT99_TX_BYTES]);

typedef enum { DMT99_RECORD_FRAME, DMT99_RECORD_UNFRAMED, DMT99_RECORD_INCOMPLETE } dmt99_record_kind;
typedef struct {
    dmt99_record_kind kind;
    uint8_t raw[DMT99_MAX_RX_BYTES], uart_errors[DMT99_MAX_RX_BYTES], length;
    dmt99_rx parsed;
} dmt99_record;
typedef void (*dmt99_record_callback)(void *context, const dmt99_record *record);
typedef struct {
    uint8_t raw[DMT99_MAX_RX_BYTES], errors[DMT99_MAX_RX_BYTES], count, display_bytes;
    dmt99_record_callback callback;
    void *context;
} dmt99_stream;
/* Callback is required and synchronous; copy data if retaining it. Do not
 * reenter this stream from its callback. Every fed byte is emitted once,
 * including noise and errors; call idle/flush at acquisition boundaries.
 * display_bytes must be explicitly 12 or 13; observed AUTO uses 12.
 */
bool dmt99_stream_init(dmt99_stream *, uint8_t display_bytes, dmt99_record_callback, void *);
void dmt99_stream_feed(dmt99_stream *, uint8_t byte, uint8_t uart_error_flags);
void dmt99_stream_idle(dmt99_stream *);
#endif
