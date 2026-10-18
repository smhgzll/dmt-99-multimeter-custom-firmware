// SPDX-License-Identifier: MIT
#include "rvswd_diag.h"
#include "rvswd_internal.h"
#include <stddef.h>
#include <string.h>

#define TRY(e) do { rv_result_t r_ = (e); if (r_ != RV_OK) return r_; } while (0)
#define SB_CONFIG 0x001f8000U
#define SB_READ32 0x00140000U // readonaddr, access=32; no autoread or increment
#define SB_ERRORS 0x00407000U

const char *rv_cap_name(rv_cap_id_t n) {
    static const char *const names[] = {"dmi", "halt", "resume", "pc", "gpr",
                                        "sram", "gpio", "usart", "spi"};
    return (unsigned)n < RV_CAP_COUNT ? names[n] : "unknown";
}
const char *rv_cap_state_name(rv_cap_state_t s) {
    return s == RV_CAP_SUPPORTED ? "supported" : s == RV_CAP_BLOCKED ? "blocked" : "unknown";
}
const char *rv_failure_class(rv_result_t r) {
    switch (r) {
    case RV_OK: return "none";
    case RV_READ_PROTECTED: return "rdp_blocked";
    case RV_AUTH_REQUIRED: return "authentication_blocked";
    case RV_TIMEOUT: case RV_POLL_TIMEOUT: return "timeout";
    case RV_IO: case RV_LINE_LOW: case RV_PARITY: case RV_NO_RESPONSE:
    case RV_WRONG_FAMILY: case RV_UNSTABLE: return "connection_error";
    case RV_ABSTRACT_ERROR: case RV_BUS_ERROR: return "access_error_cause_unproven";
    case RV_UNSUPPORTED: return "unsupported";
    case RV_POLICY: return "policy";
    case RV_HART_RUNNING: case RV_DM_INACTIVE: case RV_ABSTRACT_BUSY:
    case RV_AUTOEXEC: case RV_CLOCK_DISABLED: return "target_state";
    case RV_NOT_SELECTED: return "not_tested";
    default: return "unknown";
    }
}
static bool link_lost(rv_result_t r) {
    return r == RV_IO || r == RV_LINE_LOW || r == RV_PARITY ||
           r == RV_NO_RESPONSE || r == RV_TIMEOUT;
}
static void cap(rv_diag_report_t *o, rv_cap_id_t id, rv_result_t r) {
    o->capabilities[id] = (rv_capability_t){
        .state = r == RV_OK ? RV_CAP_SUPPORTED :
                 r == RV_AUTH_REQUIRED || r == RV_READ_PROTECTED || r == RV_UNSUPPORTED
                     ? RV_CAP_BLOCKED : RV_CAP_UNKNOWN,
        .reason = r};
}

// Deliberately small fixed profiles: no caller-selected address or byte count.
bool rv_snapshot_address_allowed(uint32_t a) {
    if ((a & 3U) != 0) return false;
    if (a >= 0x20000000U && a < 0x20000040U) return true;
    if (a == 0x40021018U || a == 0x4002101cU) return true; // RCC clock masks
    for (unsigned n = 0; n < 3; ++n) {
        uint32_t g = 0x40010800U + n * 0x400U;
        if (a >= g && a <= g + 0x0cU) return true; // no BSHR/BCR/LCKR
        const uint32_t usart[] = {0x40013800U, 0x40004400U, 0x40004800U};
        if (a >= usart[n] + 8 && a <= usart[n] + 0x14U) return true;
        const uint32_t spi[] = {0x40013000U, 0x40003800U, 0x40003c00U};
        if (a == spi[n] || a == spi[n] + 4) return true;
    }
    return false;
}
static rv_result_t hart_status(rv_bus_t *b, uint32_t *status) {
    TRY(rv_diag_read(b, 0x11, status));
    if ((*status & 0xfU) != 2 || (*status & 0xf000U)) return RV_STATUS_INVALID;
    if (!(*status & 0x80U) || (*status & 0x40U)) return RV_AUTH_REQUIRED;
    unsigned state = (*status >> 8) & 15U;
    return state == 3 || state == 12 ? RV_OK : RV_STATUS_INVALID;
}
static rv_result_t poll_hart(rv_bus_t *b, bool halted, uint32_t *status, bool running_transition) {
    unsigned running_samples = 0;
    uint64_t end = b->io.now_us(b->io.ctx) + RV_DIAG_POLL_US;
    do {
        TRY(hart_status(b, status));
        unsigned state = (*status >> 8) & 15U;
        // Generic diagnostics require resumeack. Explicit memory cycles also
        // accept three stable running samples after an observed owned halt.
        if (state == (halted ? 3U : 12U) &&
            (halted || (*status & 0x30000U) == 0x30000U)) return RV_OK;
        if (!halted && running_transition && (*status & ~0x30000U) == 0xc82U) {
            if (++running_samples == 3) return RV_OK;
        } else running_samples = 0;
        b->io.delay_us(b->io.ctx, 1000);
    } while (b->io.now_us(b->io.ctx) < end);
    return RV_POLL_TIMEOUT;
}
static rv_result_t control(rv_bus_t *b, uint32_t value) {
    uint32_t actual;
    TRY(rv_diag_write(b, 0x10, value));
    TRY(rv_diag_read(b, 0x10, &actual));
    // haltreq is W and resumereq is W1: their readback is not specified.
    // Verify readable selection/reset/active bits; DMSTATUS verifies the action.
    return (actual & 0x3fffffffU) == 1 ? RV_OK : RV_VERIFY;
}
static rv_result_t abstract_idle(rv_bus_t *b, rv_diag_report_t *o) {
    uint64_t end = b->io.now_us(b->io.ctx) + RV_DIAG_POLL_US;
    do {
        uint32_t cs;
        TRY(rv_diag_read(b, 0x16, &cs));
        o->abstract_cmderr = (cs >> 8) & 7U;
        if (!(cs & 0x1000U)) return o->abstract_cmderr ? RV_ABSTRACT_ERROR : RV_OK;
        b->io.delay_us(b->io.ctx, 1000);
    } while (b->io.now_us(b->io.ctx) < end);
    return RV_POLL_TIMEOUT;
}
static rv_result_t register_guard(rv_bus_t *b) {
    uint32_t status, automatic;
    TRY(hart_status(b, &status));
    if (((status >> 8) & 15U) != 3) return RV_HART_RUNNING;
    TRY(rv_diag_read(b, 0x18, &automatic));
    return automatic ? RV_AUTOEXEC : RV_OK;
}
static rv_result_t registers(rv_bus_t *b, rv_diag_report_t *o, bool *dirty,
                              uint32_t *saved_data) {
    b->phase = "register_snapshot";
    TRY(register_guard(b)); // never implicitly halt a running hart
    TRY(abstract_idle(b, o));
    if ((o->identity.abstractcs & 15U) == 0) return RV_UNSUPPORTED;
    TRY(rv_diag_read(b, 0x04, saved_data));
    for (unsigned i = 0; i < 33; ++i) {
        TRY(register_guard(b));
        uint32_t cmd = i == 0 ? 0x002207b1U : 0x00221000U + i - 1;
        *dirty = true; // a failed write frame may still have reached the target
        TRY(rv_diag_write(b, 0x17, cmd));
        rv_result_t r = abstract_idle(b, o);
        if (r != RV_OK) {
            if (i == 0) cap(o, RV_CAP_PC, o->abstract_cmderr == 2 ? RV_UNSUPPORTED : r);
            else cap(o, RV_CAP_GPR, o->abstract_cmderr == 2 ? RV_UNSUPPORTED : r);
            return r; // keep sticky cmderr intact; no retry or fallback
        }
        uint32_t value;
        TRY(rv_diag_read(b, 0x04, &value));
        if (i == 0) { o->pc = value; o->pc_valid = true; cap(o, RV_CAP_PC, RV_OK); }
        else { o->gpr[i - 1] = value; o->gpr_valid_mask |= 1U << (i - 1); }
    }
    cap(o, RV_CAP_GPR, RV_OK);
    return RV_OK;
}
static rv_result_t sba_idle(rv_bus_t *b, rv_diag_report_t *o) {
    uint64_t end = b->io.now_us(b->io.ctx) + RV_DIAG_POLL_US;
    do {
        TRY(rv_diag_read(b, 0x38, &o->sbcs));
        o->sbcs_valid = true;
        o->sberror = (o->sbcs >> 12) & 7U;
        if (!(o->sbcs & 0x00200000U)) {
            if (o->sberror == 1) return RV_POLL_TIMEOUT;
            return o->sbcs & SB_ERRORS ? RV_BUS_ERROR : RV_OK;
        }
        b->io.delay_us(b->io.ctx, 1000);
    } while (b->io.now_us(b->io.ctx) < end);
    return RV_POLL_TIMEOUT;
}
static rv_result_t sba_config(rv_bus_t *b, uint32_t config) {
    uint32_t cs;
    TRY(rv_diag_write(b, 0x38, config));
    TRY(rv_diag_read(b, 0x38, &cs));
    return (cs & SB_CONFIG) == config ? RV_OK : RV_VERIFY;
}
static rv_result_t snapshot(rv_bus_t *b, rv_diag_report_t *o, const char *name,
                              uint32_t address, uint32_t *out) {
    if (!rv_snapshot_address_allowed(address) || o->word_count >= RV_DIAG_MAX_WORDS)
        return RV_ARGUMENT;
    TRY(sba_idle(b, o));
    bool spi = address == 0x40013000U || address == 0x40013004U ||
        address == 0x40003800U || address == 0x40003804U ||
        address == 0x40003c00U || address == 0x40003c04U;
    if (spi && !(o->sbcs & 2U)) return RV_UNSUPPORTED; // WCH SPI controls are R16
    uint32_t config = spi ? 0x00120000U : SB_READ32;
    if ((o->sbcs & SB_CONFIG) != config) TRY(sba_config(b, config));
    TRY(rv_diag_write(b, 0x39, address)); // the ONLY target bus trigger
    TRY(sba_idle(b, o));
    uint32_t value;
    TRY(rv_diag_read(b, 0x3c, &value)); // readondata=0, no additional access
    TRY(sba_idle(b, o));
    if (spi) value &= 0xffffU;
    o->words[o->word_count++] = (rv_snapshot_word_t){name, address, value, spi ? 16U : 32U};
    if (out) *out = value;
    return RV_OK;
}
static rv_result_t peripherals(rv_bus_t *b, rv_diag_report_t *o, unsigned select) {
    uint32_t apb2, apb1;
    TRY(snapshot(b, o, "RCC.APB2PCENR", 0x40021018U, &apb2));
    TRY(snapshot(b, o, "RCC.APB1PCENR", 0x4002101cU, &apb1));
    const unsigned masks[] = {RV_DIAG_GPIO, RV_DIAG_USART, RV_DIAG_SPI};
    const rv_cap_id_t ids[] = {RV_CAP_GPIO, RV_CAP_USART, RV_CAP_SPI};
    const char *const names[3][3] = {{"GPIOA", "GPIOB", "GPIOC"},
                                    {"USART1", "USART2", "USART3"},
                                    {"SPI1", "SPI2", "SPI3"}};
    const uint32_t bases[3][3] = {{0x40010800U, 0x40010c00U, 0x40011000U},
                                 {0x40013800U, 0x40004400U, 0x40004800U},
                                 {0x40013000U, 0x40003800U, 0x40003c00U}};
    const uint32_t bits[3][3] = {{1U << 2, 1U << 3, 1U << 4},
                                {1U << 14, 1U << 17, 1U << 18},
                                {1U << 12, 1U << 14, 1U << 15}};
    for (unsigned group = 0; group < 3; ++group) {
        if (!(select & masks[group])) continue;
        unsigned read_blocks = 0;
        for (unsigned n = 0; n < 3; ++n) {
            uint32_t clocks = group == 0 || n == 0 ? apb2 : apb1;
            if (!(clocks & bits[group][n])) continue; // never turn on a clock
            for (unsigned i = 0; i < (group == 2 ? 2U : 4U); ++i) {
                rv_result_t r = snapshot(b, o, names[group][n],
                    bases[group][n] + (group == 1 ? 8U : 0U) + 4 * i, NULL);
                if (r != RV_OK) { cap(o, ids[group], r); return r; }
            }
            ++read_blocks;
        }
        cap(o, ids[group], read_blocks ? RV_OK : RV_CLOCK_DISABLED);
    }
    return RV_OK;
}
static rv_result_t memory(rv_bus_t *b, rv_diag_report_t *o, unsigned select,
                            bool *dirty, uint32_t *saved_config) {
    b->phase = "bus_snapshot";
    TRY(sba_idle(b, o));
    if ((o->sbcs >> 29) != 1 || ((o->sbcs >> 5) & 127U) != 32 || !(o->sbcs & 4U))
        return RV_UNSUPPORTED;
    // Avoid inheriting a foreign debugger's autoread/increment configuration.
    if (o->sbcs & 0x00118000U) return RV_AUTOEXEC;
    *saved_config = o->sbcs & SB_CONFIG;
    *dirty = true;
    TRY(sba_config(b, SB_READ32));
    if (select & RV_DIAG_SRAM) {
        for (unsigned n = 0; n < 16; ++n) {
            rv_result_t r = snapshot(b, o, "SRAM", 0x20000000U + 4 * n, NULL);
            if (r != RV_OK) { cap(o, RV_CAP_SRAM, r); return r; }
        }
        cap(o, RV_CAP_SRAM, RV_OK);
    }
    return select & (RV_DIAG_GPIO | RV_DIAG_USART | RV_DIAG_SPI)
        ? peripherals(b, o, select) : RV_OK;
}
static void selected_reason(rv_diag_report_t *o, unsigned select, rv_result_t r) {
    const rv_cap_id_t ids[] = {RV_CAP_PC, RV_CAP_GPR, RV_CAP_SRAM,
                              RV_CAP_GPIO, RV_CAP_USART, RV_CAP_SPI};
    const unsigned masks[] = {1, 1, 2, 4, 8, 16};
    for (unsigned i = 0; i < 6; ++i)
        if ((select & masks[i]) && o->capabilities[ids[i]].reason == RV_NOT_SELECTED)
            cap(o, ids[i], r);
}
rv_result_t rv_capabilities(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                            const rv_diag_options_t *opts, rv_diag_report_t *o) {
    if (!b || !opts || !o || !b->io.release) return RV_ARGUMENT;
    memset(o, 0, sizeof(*o));
    for (unsigned i = 0; i < RV_CAP_COUNT; ++i) cap(o, (rv_cap_id_t)i, RV_NOT_SELECTED);
    bool data_dirty = false, sb_dirty = false;
    uint32_t saved_data = 0, saved_sb = 0;
    rv_result_t r = (opts->select & ~RV_DIAG_MASK) ? RV_ARGUMENT :
        opts->halt_cycle && !opts->halt_resume_enabled ? RV_POLICY : RV_OK;
    if (opts->memory_lab_cycle && (!opts->halt_cycle || opts->select)) r = RV_POLICY;
    if (opts->memory_profile > RV_LAB_PROFILE_MAX || (opts->memory_profile && !opts->memory_lab_cycle)) r = RV_POLICY;
    if (opts->progbuf_lab_cycle && !opts->memory_lab_cycle) r = RV_POLICY;
#ifndef RV_PROGBUF_LAB
    if (opts->progbuf_lab_cycle) r = RV_POLICY;
#endif
#ifndef RV_ABSTRACT_MEMORY_LAB
    if (opts->memory_lab_cycle) r = RV_POLICY;
#endif
    if (r == RV_OK) r = rv_diag_begin(b, mv, board, wiring);
    if (r == RV_OK) r = rv_diag_preflight(b, &o->identity);
    o->abstract_cmderr = (o->identity.abstractcs >> 8) & 7U;
    o->initial_cmderr = o->abstract_cmderr;
    cap(o, RV_CAP_DMI, r);
    b->phase = "capability_status";
    b->sample = 0;
    if (r == RV_OK && o->identity.dmcontrol != 1) r = RV_DM_INACTIVE;
    if (r == RV_OK) {
        r = rv_diag_read(b, 0x18, &o->abstractauto);
        o->abstractauto_valid = r == RV_OK;
    }
    if (r == RV_OK && !opts->memory_lab_cycle) {
        r = rv_diag_read(b, 0x38, &o->sbcs); // capability/status only, never SBDATA
        o->sbcs_valid = r == RV_OK;
        if (r == RV_OK) o->sberror = (o->sbcs >> 12) & 7U;
    }
    if (r == RV_OK && opts->memory_lab_cycle) {
        b->phase = "explicit_clear_abstract_error";
        if ((o->identity.dmstatus & 0xfffffU) != 0xc82U) r = RV_STATUS_INVALID;
        else if (o->abstractauto) r = RV_AUTOEXEC;
        else r = abstract_idle(b, o);
        if (r == RV_ABSTRACT_ERROR) {
            o->cmderr_clear_attempted = true;
            b->abstract_memory_lab = true;
            r = rv_diag_write(b, 0x16, 0x700U); // W1C only while idle, explicit cycle only
            b->abstract_memory_lab = false;
            if (r == RV_OK) r = abstract_idle(b, o);
            o->cmderr_cleared = r == RV_OK;
            if (r == RV_OK) o->identity.abstractcs &= ~0x700U;
        }
    }
    if (r == RV_OK && opts->halt_cycle) {
        b->phase = "explicit_halt";
        if (((o->identity.dmstatus >> 8) & 15U) != 12) r = RV_STATUS_INVALID;
        else if (o->abstractauto) r = RV_AUTOEXEC;
        else if (o->identity.abstractcs & 0x700U) r = RV_ABSTRACT_ERROR;
        else {
            o->halt_requested = true; // ownership BEFORE the potentially partial frame
            r = control(b, 0x80000001U);
            if (r == RV_OK) r = poll_hart(b, true, &o->final_dmstatus, false);
            o->halt_observed = r == RV_OK;
            cap(o, RV_CAP_HALT, r);
            if (r == RV_OK) r = control(b, 1);
        }
    }
    if (r == RV_OK && opts->memory_lab_cycle) {
        rv_identity_t halted_id = o->identity;
        r = hart_status(b, &halted_id.dmstatus);
        if (r == RV_OK) {
            if (opts->progbuf_lab_cycle) (void)rv_progbuf_lab_active(b, &halted_id, opts->memory_profile, &o->memory_lab);
            else (void)rv_memory_lab_active(b, &halted_id, opts->memory_profile, &o->memory_lab);
            // Keep transport failures visible so outer cleanup cannot perform
            // new DMI frames on a broken link hidden by CLEANUP_FAILED.
            r = o->memory_lab.operation_result;
        }
        o->abstract_cmderr = o->memory_lab.cmderr;
    }
    if (r == RV_OK && (opts->select & RV_DIAG_REGISTERS)) {
        r = registers(b, o, &data_dirty, &saved_data);
        selected_reason(o, RV_DIAG_REGISTERS, r);
    }
    if (r == RV_OK && (opts->select & ~RV_DIAG_REGISTERS)) {
        r = memory(b, o, opts->select, &sb_dirty, &saved_sb);
        selected_reason(o, opts->select & ~RV_DIAG_REGISTERS, r);
    }
    selected_reason(o, opts->select, r == RV_OK ? RV_NOT_SELECTED : r);
    if (opts->halt_cycle && !o->halt_requested) cap(o, RV_CAP_HALT, r);
    o->operation_result = r;
    // Cleanup has its own result and never hides the original failure.
    rv_result_t cleanup = opts->memory_lab_cycle ? o->memory_lab.cleanup_result : RV_OK;
    if (link_lost(r) && (data_dirty || sb_dirty || o->halt_requested)) cleanup = RV_CLEANUP_FAILED;
    if (!link_lost(r) && data_dirty) {
        b->phase = "restore_data0";
        uint32_t cs = 0, actual = 0;
        cleanup = register_guard(b);
        if (cleanup == RV_OK) cleanup = rv_diag_read(b, 0x16, &cs);
        if (cleanup == RV_OK && (cs & 0x1000U)) cleanup = RV_POLL_TIMEOUT;
        if (cleanup == RV_OK) cleanup = rv_diag_write(b, 0x04, saved_data);
        if (cleanup == RV_OK) cleanup = rv_diag_read(b, 0x04, &actual);
        if (cleanup == RV_OK && actual != saved_data) cleanup = RV_VERIFY;
        o->data0_restored = cleanup == RV_OK;
    }
    if (!link_lost(r) && !link_lost(cleanup) && sb_dirty) {
        b->phase = "restore_sba_config";
        uint32_t cs;
        rv_result_t c = rv_diag_read(b, 0x38, &cs);
        if (c == RV_OK && (cs & 0x00200000U)) c = RV_POLL_TIMEOUT;
        if (c == RV_OK) c = sba_config(b, saved_sb); // no W1C, preserve bus-error evidence
        o->sba_config_restored = c == RV_OK;
        if (cleanup == RV_OK) cleanup = c;
    }
    if (o->halt_requested) {
        b->phase = "restore_running_state";
        rv_result_t c = RV_CLEANUP_FAILED;
        // Busy abstract execution or broken framing makes resume unsafe.
        if (!link_lost(r) && !link_lost(cleanup) && cleanup == RV_OK) {
            uint32_t cs;
            c = rv_diag_read(b, 0x16, &cs);
            if (c == RV_OK && (cs & 0x1000U)) c = RV_POLL_TIMEOUT;
            if (c == RV_OK) c = control(b, 1); // clear haltreq even after halt timeout
            if (c == RV_OK) {
                o->resume_requested = true;
                c = control(b, 0x40000001U);
            }
            if (c == RV_OK) c = poll_hart(b, false, &o->final_dmstatus, opts->memory_lab_cycle && o->halt_observed);
            o->resume_observed = c == RV_OK;
            o->resume_ack_observed = (o->final_dmstatus & 0x30000U) == 0x30000U;
            cap(o, RV_CAP_RESUME, c);
            // Clear resumereq even after a semantic timeout; never reset as fallback.
            if (!link_lost(c)) {
                rv_result_t clear = control(b, 1);
                o->control_restored = clear == RV_OK;
                if (c == RV_OK) c = clear;
            }
        }
        o->target_state_uncertain = c != RV_OK;
        if (cleanup == RV_OK) cleanup = c;
    }
    if (b->active && (r == RV_OK || data_dirty || sb_dirty || o->halt_requested) &&
        !link_lost(r) && !link_lost(cleanup)) {
        b->phase = "final_status";
        rv_result_t c = hart_status(b, &o->final_dmstatus);
        o->final_status_valid = c == RV_OK;
        if (c == RV_OK) {
            c = rv_diag_read(b, 0x10, &o->final_dmcontrol);
            o->final_control_valid = c == RV_OK;
        }
        if (cleanup == RV_OK) cleanup = c;
        if (o->halt_requested && (c != RV_OK || ((o->final_dmstatus >> 8) & 15U) != 12 ||
                                   o->final_dmcontrol != 1)) {
            o->target_state_uncertain = true;
            if (cleanup == RV_OK) cleanup = RV_STATUS_INVALID;
        }
    }
    o->pins_released = rv_release(b) == RV_OK;
    if (!o->pins_released) cleanup = RV_IO;
    o->cleanup_result = cleanup;
    return cleanup != RV_OK ? RV_CLEANUP_FAILED : r;
}
