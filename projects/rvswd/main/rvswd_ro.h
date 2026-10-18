// SPDX-License-Identifier: MIT
#pragma once
#include <stdbool.h>
#include <stdint.h>

enum { RV_SWCLK = 0, RV_SWDIO = 1, RV_MIN_MV = 3000, RV_MAX_MV = 3450 };
typedef enum {
    RV_OK, RV_ARGUMENT, RV_BOARD_UNVERIFIED, RV_WIRING_UNCHECKED,
    RV_VOLTAGE, RV_IO, RV_LINE_LOW, RV_TIMEOUT, RV_PARITY,
    RV_NO_RESPONSE, RV_WRONG_FAMILY, RV_UNSTABLE, RV_DM_INACTIVE,
    RV_AUTH_REQUIRED, RV_ABSTRACT_BUSY, RV_STATUS_INVALID,
    RV_UNSUPPORTED, RV_ABSTRACT_ERROR, RV_READ_PROTECTED, RV_OPTION_ERROR,
    RV_CAPACITY, RV_VERIFY, RV_HOST_TRANSFER, RV_CLEANUP_FAILED,
    RV_POLICY, RV_HART_RUNNING, RV_AUTOEXEC, RV_POLL_TIMEOUT, RV_BUS_ERROR,
    RV_CLOCK_DISABLED, RV_NOT_SELECTED
} rv_result_t;

// set(..., true) MUST release an open-drain line, never source current.
// release() MUST turn off both outputs AND both internal pulls, even on failure.
typedef struct {
    void *ctx;
    bool (*enable)(void *ctx);
    bool (*set)(void *ctx, unsigned line, bool released);
    bool (*get)(void *ctx, unsigned line, bool *level);
    bool (*release)(void *ctx);
    void (*delay_us)(void *ctx, uint32_t us);
    uint64_t (*now_us)(void *ctx);
} rv_io_t;
typedef struct {
    uint32_t half_us, turnaround_us;
    unsigned samples;
    bool sample_low, idle_clock_high;
} rv_config_t;
typedef struct {
    uint32_t chip_id, dmcontrol, dmstatus, hartinfo, abstractcs, cpbr;
    unsigned completed_samples;
    uint8_t last_register;
    bool last_word_valid;
    uint32_t last_word;
    bool dmactive_set, dmactive_write_attempted, dmcontrol_readback_valid;
    bool dmstatus_readback_valid, dmactive_already_active, activation_state_uncertain;
    bool pins_released;
    rv_result_t activation_operation_result, activation_cleanup_result;
    bool cpbr_attempted, cpbr_valid;
    rv_result_t cpbr_result;
    const char *cpbr_failure_stage;
} rv_identity_t;
typedef struct {
    const char *phase, *stage;
    uint32_t sequence, sample, word;
    uint8_t reg, header_aux, tail_aux, header_aux_bits, tail_aux_bits, data_bits;
    int8_t parity_expected, parity_received;
    bool write, header_parity, header_parity_sent, data_parity_sent, optional;
    rv_result_t result;
} rv_trace_t;
// Called after a frame; diagnostics buffer records and print only AFTER Hi-Z.
typedef void (*rv_trace_sink_t)(void *ctx, const rv_trace_t *trace);
typedef struct {
    rv_io_t io;
    rv_config_t config;
    bool active;
    // Private transaction scope; never enabled by ordinary diagnostics.
    bool abstract_memory_lab, progbuf_lab;
    uint64_t deadline;
    rv_trace_sink_t trace_sink;
    void *trace_ctx;
    const char *phase;
    uint32_t sequence, sample;
    rv_trace_t trace;
} rv_bus_t;

rv_result_t rv_init(rv_bus_t *bus, const rv_io_t *io, const rv_config_t *config);
rv_result_t rv_release(rv_bus_t *bus);
// No raw host-selected DMI, memory-write, erase, unlock or programming API.
// Read-only identification requires an already active DM; use rv_activate_dm separately.
rv_result_t rv_identify(rv_bus_t *bus, unsigned measured_mv,
                        bool board_verified, bool wiring_checked,
                        rv_identity_t *identity);
// Five required queries, allowing an inactive DM; optional CPBR is tried once
// after stability is established, with its raw error reported separately.
rv_result_t rv_diagnose(rv_bus_t *bus, unsigned measured_mv,
                        bool board_verified, bool wiring_checked, rv_identity_t *identity);
// Separate explicit stage: stable read-only preflight, exactly one possible
// DMCONTROL=1 write, then DMCONTROL/DMSTATUS readback. Never halts, resets,
// accesses memory or retries an uncertain write.
rv_result_t rv_activate_dm(rv_bus_t *bus, unsigned measured_mv,
                           bool board_verified, bool wiring_checked, rv_identity_t *identity);
const char *rv_result_name(rv_result_t result);

#ifdef RV_LEGACY_READER_OFFLINE_TEST
// Historical synthetic regression only; absent from the ESP32 application build.
enum { RV_FLASH_BASE = 0x08000000U, RV_FLASH_BYTES = 262144U, RV_CHUNK_BYTES = 128U };
typedef struct {
    rv_identity_t identity;
    uint32_t obr, flash_kib, bytes[2];
    rv_result_t operation_result, cleanup_result;
    uint8_t abstract_cmderr;
    bool restored, resumed, pins_released;
} rv_dump_report_t;
// Sink must yield to the RTOS and acknowledge each chunk; false aborts the read.
typedef bool (*rv_chunk_sink_t)(void *ctx, unsigned pass, uint32_t offset,
                                const uint8_t *data, unsigned length);
rv_result_t rv_dump_flash(rv_bus_t *bus, unsigned measured_mv, bool board_verified,
                          bool wiring_checked, rv_chunk_sink_t sink, void *ctx,
                          rv_dump_report_t *report);
uint32_t rv_crc32(const uint8_t *data, unsigned length);
#endif
