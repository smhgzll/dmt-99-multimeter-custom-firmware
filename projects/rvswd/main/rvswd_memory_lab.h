// SPDX-License-Identifier: MIT
#pragma once
#include "rvswd_ro.h"
enum { RV_LAB_WORDS = 8, RV_LAB_PASSES = 2, RV_LAB_PROFILE_MAX = 4 };
typedef struct {
    rv_identity_t identity;
    uint32_t words[RV_LAB_PASSES][RV_LAB_WORDS];
    unsigned completed[RV_LAB_PASSES], command_attempts, profile;
    uint32_t final_status, final_control;
    uint8_t cmderr;
    bool data_restored, final_valid, pins_released, state_uncertain;
    bool gprs_restored, program_restored, pc_unchanged;
    rv_result_t operation_result, cleanup_result;
} rv_memory_lab_report_t;
// Lab candidate only, no production readout claim. Requires an ALREADY halted
// hart. No implicit halt/resume, target writes, alias scan, or error clearing.
rv_result_t rv_memory_lab(rv_bus_t *, unsigned, bool, bool, bool,
                          rv_memory_lab_report_t *);
// Separate experiment on an UNPROTECTED SPARE only. Requires it already running;
// this tests optional running-hart support, not a validated original-target path.
rv_result_t rv_memory_lab_running_spare(rv_bus_t *, unsigned, bool, bool, bool,
                                        rv_memory_lab_report_t *);
uint32_t rv_memory_lab_address(unsigned);

// Internal: caller owns active bus, halted state, resume and pin release.
rv_result_t rv_memory_lab_active(rv_bus_t *, const rv_identity_t *, unsigned, rv_memory_lab_report_t *);

rv_result_t rv_progbuf_lab_active(rv_bus_t *, const rv_identity_t *, unsigned, rv_memory_lab_report_t *);

uint32_t rv_lab_address(unsigned profile, unsigned index);
unsigned rv_lab_word_count(unsigned profile);
