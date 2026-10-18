// SPDX-License-Identifier: MIT
#include "rvswd_memory_lab.h"
#include "rvswd_internal.h"
#include <string.h>

#ifdef RV_PROGBUF_LAB
#define TRY(e) do { rv_result_t r_ = (e); if (r_ != RV_OK) return r_; } while (0)
// lw x9,0(x8); ebreak. No store, jump, CSR write or Flash-control instruction.
static const uint32_t code[] = {0x00042483U, 0x00100073U};
typedef struct {
    rv_bus_t *b;
    rv_memory_lab_report_t *o;
    uint32_t data, program[2], gpr[2], pc;
    bool data_dirty, program_dirty, gpr_dirty, executed;
} experiment_t;
static bool lost(rv_result_t r) {
    return r == RV_IO || r == RV_LINE_LOW || r == RV_TIMEOUT || r == RV_PARITY || r == RV_NO_RESPONSE;
}
static rv_result_t guard(experiment_t *e) {
    uint32_t control, status, automatic;
    TRY(rv_diag_read(e->b, 0x10, &control));
    TRY(rv_diag_read(e->b, 0x11, &status));
    if (control != 1 || (status & 0xfffffU) != (e->o->identity.dmstatus & 0xfffffU) ||
        (status & 0xffffU) != 0x382U) return RV_STATUS_INVALID;
    TRY(rv_diag_read(e->b, 0x18, &automatic));
    return automatic ? RV_AUTOEXEC : RV_OK;
}
static rv_result_t idle(experiment_t *e, uint8_t *error) {
    uint64_t end = e->b->io.now_us(e->b->io.ctx) + 200000U;
    do {
        uint32_t cs;
        TRY(rv_diag_read(e->b, 0x16, &cs));
        if (!(cs & 0x1000U)) {
            *error = (cs >> 8) & 7;
            return *error ? RV_ABSTRACT_ERROR : RV_OK;
        }
        e->b->io.delay_us(e->b->io.ctx, 1000);
    } while (e->b->io.now_us(e->b->io.ctx) < end);
    return RV_POLL_TIMEOUT;
}
static rv_result_t command(experiment_t *e, uint32_t cmd) {
    TRY(rv_diag_write(e->b, 0x17, cmd));
    return idle(e, &e->o->cmderr);
}
static rv_result_t read_reg(experiment_t *e, uint32_t reg, uint32_t *value) {
    TRY(command(e, 0x00220000U | reg));
    return rv_diag_read(e->b, 0x04, value);
}
static rv_result_t put(experiment_t *e, uint8_t reg, uint32_t value) {
    uint32_t actual;
    TRY(rv_diag_write(e->b, reg, value));
    TRY(rv_diag_read(e->b, reg, &actual));
    return actual == value ? RV_OK : RV_VERIFY;
}
static rv_result_t write_gpr(experiment_t *e, unsigned n, uint32_t value) {
    uint32_t actual;
    TRY(put(e, 0x04, value));
    TRY(command(e, 0x00231008U + n));
    TRY(read_reg(e, 0x1008U + n, &actual));
    return actual == value ? RV_OK : RV_VERIFY;
}
static rv_result_t acquire(experiment_t *e) {
    TRY(guard(e));
    TRY(idle(e, &e->o->cmderr));
    if ((e->o->identity.abstractcs & 15U) < 1 ||
        ((e->o->identity.abstractcs >> 24) & 31U) < 2) return RV_UNSUPPORTED;
    TRY(rv_diag_read(e->b, 0x04, &e->data));
    for (unsigned n = 0; n < 2; ++n) TRY(rv_diag_read(e->b, 0x20 + n, &e->program[n]));
    e->data_dirty = true;
    TRY(read_reg(e, 0x7b1U, &e->pc));
    for (unsigned n = 0; n < 2; ++n) TRY(read_reg(e, 0x1008U + n, &e->gpr[n]));
    e->program_dirty = true;
    for (unsigned n = 0; n < 2; ++n) TRY(put(e, 0x20 + n, code[n]));
    for (unsigned pass = 0; pass < RV_LAB_PASSES; ++pass) {
        for (unsigned i = 0; i < rv_lab_word_count(e->o->profile); ++i) {
            TRY(guard(e));
            TRY(idle(e, &e->o->cmderr));
            e->gpr_dirty = true;
            TRY(write_gpr(e, 0, rv_lab_address(e->o->profile, i)));
            // Never execute a buffer that was altered or only partially written.
            for (unsigned n = 0; n < 2; ++n) {
                uint32_t actual;
                TRY(rv_diag_read(e->b, 0x20 + n, &actual));
                if (actual != code[n]) return RV_VERIFY;
            }
            e->executed = true;
            ++e->o->command_attempts;
            TRY(command(e, 0x00240000U)); // postexec only; transfer=write=0
            uint32_t value;
            TRY(read_reg(e, 0x1009U, &value));
            e->o->words[pass][i] = value;
            ++e->o->completed[pass];
            if (pass && value != e->o->words[0][i]) return RV_VERIFY;
        }
    }
    return RV_OK;
}
static rv_result_t restore(experiment_t *e) {
    TRY(guard(e));
    uint8_t error = 0;
    rv_result_t r = idle(e, &error);
    if (r == RV_ABSTRACT_ERROR) {
        // Explicit experiment cleanup only, to restore saved GPRs. No retry load.
        TRY(rv_diag_write(e->b, 0x16, 0x700U));
        r = idle(e, &error);
    }
    if (r != RV_OK) return r;
    if (e->gpr_dirty) {
        for (unsigned n = 0; n < 2; ++n) TRY(write_gpr(e, n, e->gpr[n]));
    }
    e->o->gprs_restored = true;
    if (e->program_dirty) {
        for (unsigned n = 0; n < 2; ++n) TRY(put(e, 0x20 + n, e->program[n]));
    }
    e->o->program_restored = true;
    if (e->executed) {
        uint32_t pc;
        TRY(read_reg(e, 0x7b1U, &pc));
        if (pc != e->pc) return RV_VERIFY; // never resume with an altered PC
    }
    e->o->pc_unchanged = true;
    TRY(put(e, 0x04, e->data));
    e->o->data_restored = true;
    return RV_OK;
}
#endif

rv_result_t rv_progbuf_lab_active(rv_bus_t *b, const rv_identity_t *id, unsigned profile, rv_memory_lab_report_t *o) {
    if (!b || !id || !o) return RV_ARGUMENT;
    memset(o, 0, sizeof(*o));
    o->identity = *id;
    o->profile = profile;
    if (!rv_lab_word_count(profile)) { o->operation_result = RV_ARGUMENT; return RV_ARGUMENT; }
    rv_result_t r = RV_POLICY, cleanup = RV_OK;
#ifdef RV_PROGBUF_LAB
    experiment_t e = {.b = b, .o = o};
    if (b->active) {
        b->progbuf_lab = true;
        b->phase = "fixed_load_progbuf";
        r = acquire(&e);
        uint8_t original_error = o->cmderr;
        if (e.data_dirty) {
            b->phase = "restore_progbuf_gprs";
            cleanup = lost(r) ? RV_CLEANUP_FAILED : restore(&e);
        }
        o->cmderr = original_error;
        o->state_uncertain = e.data_dirty && cleanup != RV_OK;
        b->progbuf_lab = false;
    }
#endif
    o->operation_result = r;
    o->cleanup_result = cleanup;
    return cleanup == RV_OK ? r : RV_CLEANUP_FAILED;
}
