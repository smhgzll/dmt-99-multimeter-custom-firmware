// SPDX-License-Identifier: MIT
#pragma once
#include "rvswd_ro.h"
#include "rvswd_memory_lab.h"
enum {
    RV_DIAG_REGISTERS = 1, RV_DIAG_SRAM = 2, RV_DIAG_GPIO = 4,
    RV_DIAG_USART = 8, RV_DIAG_SPI = 16, RV_DIAG_MASK = 31,
    RV_DIAG_MAX_WORDS = 64, RV_DIAG_POLL_US = 200000
};
typedef enum { RV_CAP_UNKNOWN, RV_CAP_SUPPORTED, RV_CAP_BLOCKED } rv_cap_state_t;
typedef struct { rv_cap_state_t state; rv_result_t reason; } rv_capability_t;
typedef enum { RV_CAP_DMI, RV_CAP_HALT, RV_CAP_RESUME, RV_CAP_PC, RV_CAP_GPR,
               RV_CAP_SRAM, RV_CAP_GPIO, RV_CAP_USART, RV_CAP_SPI, RV_CAP_COUNT } rv_cap_id_t;
typedef struct { const char *name; uint32_t address, value; unsigned width_bits; } rv_snapshot_word_t;
typedef struct {
    unsigned select, memory_profile;
    // Halt/resume has its own build gate and explicit halt-cycle request.
    bool halt_cycle, halt_resume_enabled;
    bool progbuf_lab_cycle;
    bool memory_lab_cycle; // requires halt gate AND compiled lab, excludes snapshots
} rv_diag_options_t;
typedef struct {
    rv_identity_t identity;
    rv_memory_lab_report_t memory_lab;
    bool cmderr_clear_attempted, cmderr_cleared;
    uint8_t initial_cmderr;
    rv_capability_t capabilities[RV_CAP_COUNT];
    uint32_t abstractauto, sbcs, final_dmstatus, final_dmcontrol;
    bool abstractauto_valid, sbcs_valid, final_status_valid, final_control_valid;
    uint32_t pc, gpr[32], gpr_valid_mask;
    bool pc_valid, halt_requested, halt_observed, resume_requested, resume_observed;
    bool resume_ack_observed;
    bool control_restored, data0_restored, sba_config_restored, pins_released;
    bool target_state_uncertain;
    uint8_t abstract_cmderr, sberror;
    rv_result_t operation_result, cleanup_result;
    rv_snapshot_word_t words[RV_DIAG_MAX_WORDS];
    unsigned word_count;
} rv_diag_report_t;
rv_result_t rv_capabilities(rv_bus_t *, unsigned, bool, bool,
                            const rv_diag_options_t *, rv_diag_report_t *);
const char *rv_cap_name(rv_cap_id_t);
const char *rv_cap_state_name(rv_cap_state_t);
const char *rv_failure_class(rv_result_t);
