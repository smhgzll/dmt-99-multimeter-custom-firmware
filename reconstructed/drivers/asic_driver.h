#ifndef DMT_ASIC_DRIVER_H
#define DMT_ASIC_DRIVER_H
#include "measurement_asic.h"
/* Portable DMT99 peer protocol, without display, settings, MMIO or ASIC model.
 * transmit returns local transport acceptance, never a peer acknowledgement.
 * A caller supplies a complete UART-IDLE snapshot and its actual received size.
 */
typedef struct {
    void *context;
    bool (*transmit)(void *,const uint8_t packet[MEASUREMENT_TX_BYTES]);
    void (*delay_ms)(void *,uint32_t);
} dmt_asic_io;
enum dmt_asic_tx_kind {
    DMT_ASIC_TX_INVALID, DMT_ASIC_TX_COMMAND, DMT_ASIC_TX_LEGACY_ZERO,
    DMT_ASIC_TX_DIAGNOSTIC_A0
};
enum dmt_asic_tx_kind asic_classify_tx(const uint8_t *,size_t);
bool asic_transmit_observed(const dmt_asic_io *,const uint8_t *,size_t);
bool asic_request_mode(const dmt_asic_io *,uint8_t mode); /* observed09..16 */
bool asic_request_hold(const dmt_asic_io *); /* observed03 button action; not force-HOLD-on */
bool asic_forward_key(const dmt_asic_io *,uint8_t key);   /* keys1..4, no debounce */
/* Explicit opt-in replay of B56A. A0 has no established functional meaning. */
bool asic_replay_diagnostic_entry(const dmt_asic_io *);
measurement_frame_kind asic_receive_idle(const uint8_t *,size_t,measurement_frame *);
/* Reads LCD text/value using the original host parser, including its numeric
 * flag quirks. mode/numeric are caller-owned context, not confirmed ASIC state. */
bool asic_reading_update(measurement_reading *,const uint8_t *,size_t);
#endif
