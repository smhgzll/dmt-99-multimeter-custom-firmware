// SPDX-License-Identifier: MIT
#include "rvswd_memory_lab.h"
#include "rvswd_internal.h"
#include <string.h>

uint32_t rv_memory_lab_address(unsigned i) {
    // Known application first, then missing prefix. No settings/alias/ROM reads.
    return i < 4 ? 0x08006400U + i * 4 :
           i < RV_LAB_WORDS ? 0x08000000U + (i - 4) * 4 : UINT32_MAX;
}
unsigned rv_lab_word_count(unsigned profile) {
    return profile == 0 ? RV_LAB_WORDS : profile <= RV_LAB_PROFILE_MAX ? 1 : 0;
}
uint32_t rv_lab_address(unsigned profile, unsigned i) {
    static const uint32_t word_address[] = {0x08000000U, 0x00000000U, 0x00006400U, 0x20000000U};
    if (!profile) return rv_memory_lab_address(i);
    return profile <= RV_LAB_PROFILE_MAX && i == 0 ? word_address[profile - 1] : UINT32_MAX;
}
#ifdef RV_ABSTRACT_MEMORY_LAB
#define TRY(e) do { rv_result_t r_ = (e); if (r_ != RV_OK) return r_; } while (0)
static bool lost(rv_result_t r) {
    return r == RV_IO || r == RV_LINE_LOW || r == RV_TIMEOUT ||
           r == RV_PARITY || r == RV_NO_RESPONSE;
}
static rv_result_t guard(rv_bus_t *b, uint32_t baseline, bool running_spare, uint32_t *status,
                         uint32_t *control) {
    uint32_t automatic;
    TRY(rv_diag_read(b, 0x10, control));
    if (*control != 1) return RV_DM_INACTIVE;
    TRY(rv_diag_read(b, 0x11, status));
    if ((*status & 0xfffffU) != (baseline & 0xfffffU)) return RV_UNSTABLE;
    if ((*status & 0xffffU) != (running_spare ? 0xc82U : 0x382U)) return RV_STATUS_INVALID;
    TRY(rv_diag_read(b, 0x18, &automatic));
    return automatic ? RV_AUTOEXEC : RV_OK;
}
static rv_result_t idle(rv_bus_t *b, uint8_t *error) {
    uint64_t end = b->io.now_us(b->io.ctx) + 200000U;
    do {
        uint32_t cs;
        TRY(rv_diag_read(b, 0x16, &cs));
        if (!(cs & 0x1000U)) {
            *error = (cs >> 8) & 7U;
            return *error ? RV_ABSTRACT_ERROR : RV_OK;
        }
        b->io.delay_us(b->io.ctx, 1000);
    } while (b->io.now_us(b->io.ctx) < end);
    return RV_POLL_TIMEOUT;
}
static rv_result_t put_checked(rv_bus_t *b, uint8_t reg, uint32_t value) {
    uint32_t actual;
    TRY(rv_diag_write(b, reg, value));
    TRY(rv_diag_read(b, reg, &actual));
    return actual == value ? RV_OK : RV_VERIFY;
}
static rv_result_t acquire(rv_bus_t *b, rv_memory_lab_report_t *o,
                           bool running_spare, uint32_t saved[2], bool *dirty) {
    uint32_t status, control;
    TRY(guard(b, o->identity.dmstatus, running_spare, &status, &control));
    TRY(idle(b, &o->cmderr));
    if ((o->identity.abstractcs & 15U) < 2) return RV_UNSUPPORTED;
    TRY(rv_diag_read(b, 0x04, &saved[0]));
    TRY(rv_diag_read(b, 0x05, &saved[1]));
    b->abstract_memory_lab = true;
    for (unsigned p = 0; p < RV_LAB_PASSES; ++p) {
        for (unsigned i = 0; i < rv_lab_word_count(o->profile); ++i) {
            TRY(guard(b, o->identity.dmstatus, running_spare, &status, &control));
            TRY(idle(b, &o->cmderr));
            *dirty = true; // partial write might already have reached DM
            TRY(put_checked(b, 0x05, rv_lab_address(o->profile, i)));
            ++o->command_attempts;
            // cmdtype=2, physical, 32-bit, read. postexec/postincrement=0.
            TRY(rv_diag_write(b, 0x17, 0x02200000U));
            TRY(idle(b, &o->cmderr));
            uint32_t value;
            TRY(rv_diag_read(b, 0x04, &value));
            o->words[p][i] = value;
            ++o->completed[p];
            if (p && value != o->words[0][i]) return RV_VERIFY;
        }
    }
    return RV_OK; // transport agreement only; host must check reference and staleness
}
#endif
static rv_result_t run(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                       bool explicit_lab, bool running_spare, const rv_identity_t *active_id, unsigned profile, rv_memory_lab_report_t *o) {
    if (!b || !o || !b->io.release) return RV_ARGUMENT;
    memset(o, 0, sizeof(*o));
    o->profile = profile;
    if (!rv_lab_word_count(profile)) return RV_ARGUMENT;
    rv_result_t r = RV_POLICY, cleanup = RV_OK;
#ifdef RV_ABSTRACT_MEMORY_LAB
    bool dirty = false;
    uint32_t saved[2] = {0};
    if (active_id && explicit_lab && b->active) { o->identity = *active_id; r = RV_OK; }
    else {
        if (explicit_lab) r = rv_diag_begin(b, mv, board, wiring);
        if (r == RV_OK) r = rv_diag_preflight(b, &o->identity);
    }
    if (r == RV_OK && (o->identity.dmstatus & 0xf00U) != (running_spare ? 0xc00U : 0x300U))
        r = running_spare ? RV_STATUS_INVALID : RV_HART_RUNNING;
    b->phase = "lab_abstract_memory";
    b->sample = 0;
    if (r == RV_OK) r = acquire(b, o, running_spare, saved, &dirty);
    if (dirty) {
        cleanup = RV_CLEANUP_FAILED;
        if (!lost(r)) {
            b->phase = "lab_restore_data";
            cleanup = guard(b, o->identity.dmstatus, running_spare, &o->final_status, &o->final_control);
            if (cleanup == RV_OK) {
                uint8_t error = 0;
                cleanup = idle(b, &error);
                // Sticky cmderr is retained. Restoring idle DATA is safe with
                // autoexec disabled; no new command or error-clear write.
                if (cleanup == RV_ABSTRACT_ERROR) cleanup = RV_OK;
            }
            if (cleanup == RV_OK) cleanup = put_checked(b, 0x05, saved[1]);
            if (cleanup == RV_OK) cleanup = put_checked(b, 0x04, saved[0]);
            o->data_restored = cleanup == RV_OK;
        }
    }
    if (b->active && !lost(r) && !lost(cleanup) && cleanup == RV_OK) {
        b->phase = "lab_final_status";
        rv_result_t c = guard(b, o->identity.dmstatus, running_spare, &o->final_status, &o->final_control);
        o->final_valid = c == RV_OK;
        if (dirty || r == RV_OK) cleanup = c;
    }
    o->state_uncertain = dirty && (!o->final_valid || !o->data_restored);
#else
    (void)active_id; (void)mv; (void)board; (void)wiring; (void)explicit_lab; (void)running_spare;
#endif
    o->operation_result = r;
    if (!active_id) {
        o->pins_released = rv_release(b) == RV_OK;
        if (!o->pins_released) cleanup = RV_IO;
    }
    o->cleanup_result = cleanup;
    return cleanup == RV_OK ? r : RV_CLEANUP_FAILED;
}
rv_result_t rv_memory_lab(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                          bool explicit_lab, rv_memory_lab_report_t *o) {
    return run(b, mv, board, wiring, explicit_lab, false, NULL, 0, o);
}
rv_result_t rv_memory_lab_running_spare(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                                        bool explicit_lab, rv_memory_lab_report_t *o) {
    return run(b, mv, board, wiring, explicit_lab, true, NULL, 0, o);
}

rv_result_t rv_memory_lab_active(rv_bus_t *b, const rv_identity_t *id, unsigned profile, rv_memory_lab_report_t *o) {
    if (!id) return RV_ARGUMENT;
    return run(b, 0, false, false, true, false, id, profile, o);
}
