// SPDX-License-Identifier: MIT
// Experimental WCH short-frame read transport. See docs/AUDIT.md for sources.
#include "rvswd_ro.h"
#include "rvswd_internal.h"
#include <stddef.h>
#include <string.h>

#define TRY(expr) do { rv_result_t r_ = (expr); if (r_ != RV_OK) return r_; } while (0)
#define FRAME_TIMEOUT_US 100000U

const char *rv_result_name(rv_result_t r) {
    static const char *const names[] = {
        "ok", "invalid_argument", "board_unverified", "wiring_unchecked",
        "voltage_out_of_range", "gpio_error", "line_held_low", "frame_timeout",
        "parity_error", "no_response", "unexpected_device_family", "unstable_identity",
        "debug_module_inactive", "authentication_required", "abstract_command_busy",
        "invalid_debug_status", "unsupported_debug_capability", "abstract_command_error",
        "read_protection_enabled", "option_byte_error", "insufficient_flash_capacity",
        "debug_readback_mismatch", "host_transfer_error", "cleanup_unconfirmed",
        "disabled_by_policy", "hart_running_no_implicit_halt", "autoexec_armed",
        "poll_timeout", "system_bus_error", "peripheral_clock_disabled", "not_selected"
    };
    return (unsigned)r < sizeof(names) / sizeof(names[0]) ? names[r] : "unknown_error";
}

rv_result_t rv_release(rv_bus_t *b) {
    if (!b || !b->io.release) return RV_ARGUMENT;
    b->active = false;
    b->abstract_memory_lab = false;
    b->progbuf_lab = false;
    return b->io.release(b->io.ctx) ? RV_OK : RV_IO;
}

rv_result_t rv_init(rv_bus_t *b, const rv_io_t *io, const rv_config_t *cfg) {
    if (!b || !io || !cfg || !io->enable || !io->set || !io->get ||
        !io->release || !io->delay_us || !io->now_us) return RV_ARGUMENT;
    memset(b, 0, sizeof(*b));
    b->io = *io;
    b->config = *cfg;
    TRY(rv_release(b));
    if (cfg->half_us < 10 || cfg->half_us > 250 ||
        cfg->turnaround_us < 2 || cfg->turnaround_us > 200 ||
        cfg->samples < 3 || cfg->samples > 16) return RV_ARGUMENT;
    return RV_OK;
}

static rv_result_t check_time(rv_bus_t *b) {
    if (!b->active) return RV_ARGUMENT;
    return b->io.now_us(b->io.ctx) < b->deadline ? RV_OK : RV_TIMEOUT;
}
static rv_result_t set(rv_bus_t *b, unsigned line, bool high) {
    TRY(check_time(b));
    return b->io.set(b->io.ctx, line, high) ? RV_OK : RV_IO;
}
static rv_result_t sample(rv_bus_t *b, unsigned line, bool *v) {
    TRY(check_time(b));
    return b->io.get(b->io.ctx, line, v) ? RV_OK : RV_IO;
}
static rv_result_t wait_us(rv_bus_t *b, uint32_t us) {
    b->io.delay_us(b->io.ctx, us);
    return check_time(b);
}
static rv_result_t high_clock(rv_bus_t *b) {
    bool high;
    TRY(set(b, RV_SWCLK, true));
    TRY(wait_us(b, b->config.half_us));
    TRY(sample(b, RV_SWCLK, &high));
    return high ? RV_OK : RV_LINE_LOW;
}
static rv_result_t send_bit(rv_bus_t *b, bool value) {
    TRY(set(b, RV_SWCLK, false));
    TRY(set(b, RV_SWDIO, value)); // change data only while clock is LOW
    TRY(wait_us(b, b->config.half_us));
    return high_clock(b);
}
static rv_result_t receive_bit(rv_bus_t *b, bool *value) {
    TRY(set(b, RV_SWCLK, false));
    TRY(set(b, RV_SWDIO, true)); // output transistor off for the whole target phase
    TRY(wait_us(b, b->config.half_us));
    if (b->config.sample_low) TRY(sample(b, RV_SWDIO, value));
    TRY(high_clock(b));
    if (!b->config.sample_low) TRY(sample(b, RV_SWDIO, value));
    return RV_OK;
}
static rv_result_t handover(rv_bus_t *b) {
    TRY(set(b, RV_SWCLK, false));
    TRY(set(b, RV_SWDIO, true));
    return wait_us(b, b->config.turnaround_us);
}
static rv_result_t start(rv_bus_t *b) {
    TRY(set(b, RV_SWDIO, true));
    TRY(high_clock(b));
    bool high;
    TRY(sample(b, RV_SWDIO, &high));
    if (!high) return RV_LINE_LOW;
    TRY(set(b, RV_SWDIO, false)); // START: data falls while clock is HIGH
    TRY(wait_us(b, b->config.half_us));
    return set(b, RV_SWCLK, false);
}
static rv_result_t stop(rv_bus_t *b) {
    TRY(set(b, RV_SWCLK, false));
    TRY(set(b, RV_SWDIO, false));
    TRY(wait_us(b, b->config.half_us));
    TRY(high_clock(b)); // terminal clock: 52 frame clocks + this edge = 53
    TRY(set(b, RV_SWDIO, true)); // STOP: data rises while clock is HIGH
    TRY(wait_us(b, b->config.half_us));
    if (!b->config.idle_clock_high) TRY(set(b, RV_SWCLK, false));
    return RV_OK;
}
static rv_result_t sync_link(rv_bus_t *b) {
    b->deadline = b->io.now_us(b->io.ctx) + FRAME_TIMEOUT_US;
    TRY(set(b, RV_SWDIO, true));
    TRY(high_clock(b));
    bool high;
    TRY(sample(b, RV_SWDIO, &high));
    if (!high) return RV_LINE_LOW;
    // Protocol line synchronization only. No NRST or DMCONTROL reset request.
    for (unsigned i = 0; i < 100; ++i) {
        TRY(set(b, RV_SWCLK, false));
        TRY(wait_us(b, b->config.half_us));
        TRY(high_clock(b));
    }
    return stop(b);
}

static bool allowed_register(uint8_t reg) {
    return reg == 0x7f || reg == 0x10 || reg == 0x11 || reg == 0x12 ||
           reg == 0x16 || reg == 0x7c || reg == 0x18 || reg == 0x04 ||
           reg == 0x05 || (reg >= 0x20 && reg <= 0x22) ||
           reg == 0x38 || reg == 0x39 || reg == 0x3c;
}
static void trace_begin(rv_bus_t *b, uint8_t reg, bool write, uint32_t word) {
    b->trace = (rv_trace_t){.phase = b->phase ? b->phase : "reader",
        .stage = "start", .sequence = ++b->sequence, .sample = b->sample,
        .reg = reg, .write = write, .word = word,
        .optional = reg == 0x7c,
        .parity_expected = -1, .parity_received = -1};
}
static rv_result_t trace_end(rv_bus_t *b, rv_result_t r) {
    b->trace.result = r;
    if (r == RV_OK) b->trace.stage = "complete";
    if (b->trace_sink) b->trace_sink(b->trace_ctx, &b->trace);
    return r;
}
static rv_result_t receive_aux(rv_bus_t *b, bool tail) {
    uint8_t *value = tail ? &b->trace.tail_aux : &b->trace.header_aux;
    uint8_t *bits = tail ? &b->trace.tail_aux_bits : &b->trace.header_aux_bits;
    for (unsigned i = 0; i < 3; ++i) {
        bool v;
        TRY(receive_bit(b, &v));
        *value = (*value << 1) | (unsigned)v;
        ++*bits;
    }
    return RV_OK;
}
static rv_result_t read_frame(rv_bus_t *b, uint8_t reg, uint32_t *out) {
    if (!out || !allowed_register(reg)) return RV_ARGUMENT;
    b->deadline = b->io.now_us(b->io.ctx) + FRAME_TIMEOUT_US;
    TRY(start(b));
    bool parity = false;
    b->trace.stage = "header_address";
    for (int bit = 6; bit >= 0; --bit) {
        bool v = (reg >> bit) & 1U;
        parity ^= v;
        TRY(send_bit(b, v));
    }
    b->trace.stage = "header_opcode";
    TRY(send_bit(b, false));
    b->trace.stage = "header_parity";
    b->trace.header_parity = parity;
    TRY(send_bit(b, parity));
    b->trace.header_parity_sent = true;
    // cnlohr short-frame ownership: target aux3, host control2 (00).
    // Aux bits are recorded by a logic analyzer, not interpreted as SWD ACKs.
    b->trace.stage = "header_turnaround";
    TRY(handover(b));
    b->trace.stage = "header_aux";
    TRY(receive_aux(b, false));
    b->trace.stage = "header_return_turnaround";
    TRY(wait_us(b, b->config.turnaround_us));
    b->trace.stage = "control_00";
    TRY(send_bit(b, false));
    TRY(send_bit(b, false));
    b->trace.stage = "data_turnaround";
    TRY(handover(b));
    uint32_t word = 0;
    parity = false;
    b->trace.stage = "data";
    for (unsigned i = 0; i < 32; ++i) {
        bool v;
        TRY(receive_bit(b, &v));
        word = (word << 1) | (uint32_t)v;
        parity ^= v;
        b->trace.word = word;
        b->trace.data_bits = i + 1;
    }
    bool received_parity;
    b->trace.stage = "data_parity";
    b->trace.parity_expected = parity;
    TRY(receive_bit(b, &received_parity));
    b->trace.parity_received = received_parity;
    b->trace.stage = "tail_aux";
    TRY(receive_aux(b, true));
    b->trace.stage = "tail_return_turnaround";
    TRY(wait_us(b, b->config.turnaround_us));
    b->trace.stage = "control_10";
    TRY(send_bit(b, true));
    TRY(send_bit(b, false));
    b->trace.stage = "stop";
    TRY(stop(b));
    // Preserve raw data for error reports, but never treat a parity failure as valid.
    *out = word;
    b->trace.stage = "chip_id_check";
    if (reg == 0x7f && word == UINT32_MAX) return RV_NO_RESPONSE;
    b->trace.stage = "data_parity_check";
    return parity == received_parity ? RV_OK : RV_PARITY;
}
static rv_result_t read_register(rv_bus_t *b, uint8_t reg, uint32_t *out) {
    trace_begin(b, reg, false, 0);
    return trace_end(b, read_frame(b, reg, out));
}

// Private, bounded debug-register writes only. There is no target memory-write
// primitive. No CFGR keys, reset requests, Flash controller writes or algorithms.
static rv_result_t write_frame(rv_bus_t *b, uint8_t reg, uint32_t word) {
    bool allowed = reg == 0x04; // DATA0 restoration; never a CPU register write
    if (reg == 0x10) allowed = word == 1 || word == 0x80000001U || word == 0x40000001U;
    if (reg == 0x17) allowed = (word >= 0x00221000U && word <= 0x0022101fU) ||
        word == 0x002207b1U; // transfer read only; postexec/write/postincrement = 0
    if (reg == 0x38) allowed = !(word & ~0x001e0000U); // config only, no error W1C
    if (reg == 0x39) allowed = rv_snapshot_address_allowed(word);
#ifdef RV_ABSTRACT_MEMORY_LAB
    if (b->abstract_memory_lab && reg == 0x16) allowed = word == 0x700U;
    if (b->abstract_memory_lab && reg == 0x05) allowed = true; // address / saved DATA1
    if (b->abstract_memory_lab && reg == 0x17 && word == 0x02200000U) allowed = true;
#endif
#ifdef RV_PROGBUF_LAB
    if (b->progbuf_lab && (reg == 0x20 || reg == 0x21)) allowed = true;
    if (b->progbuf_lab && reg == 0x16) allowed = word == 0x700U;
    if (b->progbuf_lab && reg == 0x17 && (word == 0x00231008U || word == 0x00231009U ||
                                        word == 0x00240000U)) allowed = true;
#endif
#ifdef RV_LEGACY_READER_OFFLINE_TEST
    if (reg == 0x05 || (reg >= 0x20 && reg <= 0x22)) allowed = true;
    if (reg == 0x16) allowed = word == 0x700U;
    if (reg == 0x17) allowed = allowed || word == 0x00240000U ||
        (word >= 0x00221008U && word <= 0x0022100bU) ||
        (word >= 0x00231008U && word <= 0x0023100bU);
#endif
    if (!allowed) return RV_ARGUMENT;
    b->deadline = b->io.now_us(b->io.ctx) + FRAME_TIMEOUT_US;
    TRY(start(b));
    bool parity = true; // even parity over address + write opcode (1)
    b->trace.stage = "header_address";
    for (int bit = 6; bit >= 0; --bit) {
        bool v = (reg >> bit) & 1U;
        parity ^= v;
        TRY(send_bit(b, v));
    }
    b->trace.stage = "header_opcode";
    TRY(send_bit(b, true));
    b->trace.stage = "header_parity";
    b->trace.header_parity = parity;
    TRY(send_bit(b, parity));
    b->trace.header_parity_sent = true;
    b->trace.stage = "header_turnaround";
    TRY(handover(b));
    b->trace.stage = "header_aux";
    TRY(receive_aux(b, false));
    b->trace.stage = "header_return_turnaround";
    TRY(wait_us(b, b->config.turnaround_us));
    b->trace.stage = "control_00";
    TRY(send_bit(b, false));
    TRY(send_bit(b, false));
    parity = false;
    b->trace.stage = "data";
    for (int bit = 31; bit >= 0; --bit) {
        bool v = (word >> bit) & 1U;
        parity ^= v;
        TRY(send_bit(b, v));
        ++b->trace.data_bits;
    }
    b->trace.stage = "data_parity";
    b->trace.parity_expected = parity;
    TRY(send_bit(b, parity));
    b->trace.data_parity_sent = true;
    b->trace.stage = "tail_turnaround";
    TRY(handover(b));
    b->trace.stage = "tail_aux";
    TRY(receive_aux(b, true));
    b->trace.stage = "tail_return_turnaround";
    TRY(wait_us(b, b->config.turnaround_us));
    b->trace.stage = "control_10";
    TRY(send_bit(b, true));
    TRY(send_bit(b, false));
    // Short frames have no verified ACK. Callers verify readback/completion.
    b->trace.stage = "stop";
    return stop(b);
}
static rv_result_t write_register(rv_bus_t *b, uint8_t reg, uint32_t word) {
    trace_begin(b, reg, true, word);
    return trace_end(b, write_frame(b, reg, word));
}

static rv_result_t probe(rv_bus_t *b, rv_identity_t *out, bool require_active) {
    TRY(sync_link(b));
    rv_identity_t baseline = {0};
    for (unsigned n = 0; n < b->config.samples; ++n) {
        rv_identity_t cur = {0};
        b->sample = n + 1;
        const uint8_t regs[] = {0x7f, 0x10, 0x11, 0x12, 0x16};
        uint32_t *const words[] = {&cur.chip_id, &cur.dmcontrol, &cur.dmstatus,
                                   &cur.hartinfo, &cur.abstractcs};
        for (unsigned i = 0; i < sizeof(regs); ++i) {
            out->last_register = regs[i];
            out->last_word_valid = false;
            uint32_t v = 0;
            rv_result_t r = read_register(b, regs[i], &v);
            out->last_word = v;
            if (r != RV_OK) return r;
            out->last_word_valid = true;
            *words[i] = v;
            cur.completed_samples = n;
            cur.last_register = regs[i];
            cur.last_word_valid = true;
            cur.last_word = v;
            *out = cur; // retain successful earlier queries if a later read fails
        }
        cur.completed_samples = n + 1;
        cur.last_register = out->last_register;
        cur.last_word_valid = true;
        cur.last_word = out->last_word;
        *out = cur;
        if (!cur.chip_id || !cur.dmstatus) return RV_NO_RESPONSE;
        // A family hint only; 0x7f alone does not prove the RCT6 package/capacity.
        if ((cur.chip_id & 0xfff00500U) != 0x30300500U) return RV_WRONG_FAMILY;
        if ((cur.dmstatus & 0xfU) != 2U) return RV_STATUS_INVALID;
        if (!(cur.dmstatus & (1U << 7)) || (cur.dmstatus & (1U << 6)))
            return RV_AUTH_REQUIRED;
        if (cur.dmstatus & 0x0000f000U) return RV_STATUS_INVALID; // unavailable/nonexistent
        // This target is single-hart: paired any/all flags must agree, and it
        // cannot be simultaneously halted and running (or report neither).
        unsigned hart_state = (cur.dmstatus >> 8) & 0xfU;
        if (hart_state != 3 && hart_state != 12) return RV_STATUS_INVALID;
        if (cur.abstractcs & (1U << 12)) return RV_ABSTRACT_BUSY;
        if (cur.dmcontrol != 0 && cur.dmcontrol != 1) return RV_STATUS_INVALID;
        *out = cur;
        if (n && (cur.chip_id != baseline.chip_id || cur.hartinfo != baseline.hartinfo ||
                  cur.dmcontrol != baseline.dmcontrol ||
                  cur.abstractcs != baseline.abstractcs ||
                  ((cur.dmstatus ^ baseline.dmstatus) & 0xffffU))) return RV_UNSTABLE;
        baseline = cur;
    }
    return require_active && !out->dmcontrol ? RV_DM_INACTIVE : RV_OK;
}

static rv_result_t optional_cpbr(rv_bus_t *b, rv_identity_t *out) {
    // 0x7c is a vendor capability register, not needed by this reader. No
    // CH32V303 inactive-DM access guarantee was found. Keep the failed frame
    // visible, never retry it, and perform no later DMI operation in this request.
    b->phase = "optional_cpbr";
    b->sample = 0;
    out->cpbr_attempted = true;
    out->last_register = 0x7c;
    out->last_word_valid = false;
    out->cpbr_result = read_register(b, 0x7c, &out->cpbr);
    out->last_word = out->cpbr;
    out->cpbr_valid = out->cpbr_result == RV_OK;
    out->last_word_valid = out->cpbr_valid;
    out->cpbr_failure_stage = out->cpbr_valid ? "none" : b->trace.stage;
    // Only the optional register's parity/no-response failures are nonfatal.
    // GPIO, clock, timeout and held-low failures still abort the command.
    if (out->cpbr_result == RV_PARITY || out->cpbr_result == RV_NO_RESPONSE) return RV_OK;
    return out->cpbr_result;
}

static rv_result_t prepare(rv_bus_t *b, unsigned mv, bool board, bool wiring) {
    b->sequence = b->sample = 0;
    b->phase = "legacy_read_only";
    TRY(rv_release(b));
    if (!board) return RV_BOARD_UNVERIFIED;
    if (!wiring) return RV_WIRING_UNCHECKED;
    if (mv < RV_MIN_MV || mv > RV_MAX_MV) return RV_VOLTAGE;
    if (!b->io.enable(b->io.ctx)) { rv_release(b); return RV_IO; }
    b->active = true;
    return RV_OK;
}

static rv_result_t read_only_probe(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                                   rv_identity_t *out, bool require_active) {
    if (!b || !out || !b->io.release) return RV_ARGUMENT;
    memset(out, 0, sizeof(*out));
    TRY(prepare(b, mv, board, wiring));
    rv_result_t r = probe(b, out, require_active);
    if (r == RV_OK || r == RV_DM_INACTIVE) {
        rv_result_t optional = optional_cpbr(b, out);
        if (optional != RV_OK) r = optional;
    }
    rv_result_t cleanup = rv_release(b); // every path, success or failure, ends Hi-Z
    out->pins_released = cleanup == RV_OK;
    return cleanup == RV_OK ? r : cleanup;
}
rv_result_t rv_identify(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                        rv_identity_t *out) {
    return read_only_probe(b, mv, board, wiring, out, true);
}
rv_result_t rv_diagnose(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                        rv_identity_t *out) {
    return read_only_probe(b, mv, board, wiring, out, false);
}
static rv_result_t activate_dm(rv_bus_t *b, rv_identity_t *out) {
    b->phase = "activation_preflight";
    TRY(probe(b, out, false)); // five stable required queries; no CPBR before writing
    uint32_t preflight_status = out->dmstatus;
    if (out->dmcontrol == 1) out->dmactive_already_active = true;
    else {
        b->sample = 0;
        b->phase = "dmactive_write";
        out->last_register = 0x10;
        out->last_word = 1;
        out->last_word_valid = false;
        out->dmactive_write_attempted = true; // no retry if this frame is uncertain
        rv_result_t wr = write_register(b, 0x10, 1); // sole activation write value
        if (wr != RV_OK) { out->activation_state_uncertain = true; return wr; }
    }
    b->phase = "dmcontrol_readback";
    out->last_register = 0x10;
    out->last_word_valid = false;
    rv_result_t r = read_register(b, 0x10, &out->last_word);
    if (r != RV_OK) { out->activation_state_uncertain = out->dmactive_write_attempted; return r; }
    out->last_word_valid = true;
    out->dmcontrol_readback_valid = true;
    out->dmcontrol = out->last_word;
    if (out->dmcontrol != 1) return RV_DM_INACTIVE;
    out->dmactive_set = true;
    b->phase = "dmstatus_readback";
    out->last_register = 0x11;
    out->last_word_valid = false;
    r = read_register(b, 0x11, &out->last_word);
    if (r != RV_OK) { out->activation_state_uncertain = true; return r; }
    out->last_word_valid = true;
    out->dmstatus_readback_valid = true;
    out->dmstatus = out->last_word;
    // Activation must preserve the hart's paired running/halted state and
    // report the same authenticated single-hart status seen in preflight.
    if ((out->dmstatus & 0xfU) != 2U || !(out->dmstatus & (1U << 7)) ||
        (out->dmstatus & (1U << 6)) || (out->dmstatus & 0x0000f000U) ||
        (((out->dmstatus >> 8) & 0xfU) != 3 && ((out->dmstatus >> 8) & 0xfU) != 12) ||
        (((out->dmstatus ^ preflight_status) & 0x00000f80U) != 0))
        return RV_STATUS_INVALID;
    return RV_OK;
}
rv_result_t rv_activate_dm(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                           rv_identity_t *out) {
    if (!b || !out || !b->io.release) return RV_ARGUMENT;
    memset(out, 0, sizeof(*out));
    rv_result_t r = prepare(b, mv, board, wiring);
    if (r == RV_OK) r = activate_dm(b, out);
    rv_result_t cleanup = rv_release(b);
    out->activation_operation_result = r;
    out->activation_cleanup_result = cleanup;
    out->pins_released = cleanup == RV_OK;
    return cleanup == RV_OK ? r : RV_CLEANUP_FAILED;
}

// Internal transport entry points: deliberately not a raw host command surface.
rv_result_t rv_diag_begin(rv_bus_t *b, unsigned mv, bool board, bool wiring) {
    return prepare(b, mv, board, wiring);
}
rv_result_t rv_diag_preflight(rv_bus_t *b, rv_identity_t *id) {
    return probe(b, id, false);
}
rv_result_t rv_diag_read(rv_bus_t *b, uint8_t reg, uint32_t *value) {
    return read_register(b, reg, value);
}
rv_result_t rv_diag_write(rv_bus_t *b, uint8_t reg, uint32_t value) {
    return write_register(b, reg, value);
}

#ifdef RV_LEGACY_READER_OFFLINE_TEST
// Read engine from cnlohr's CH32V20x/V30x program-buffer sequence, with
// https://github.com/cnlohr/esp32s2-cookbook/blob/master/ch32v003programmer/main/ch32v003_swio.h
// ABSTRACTAUTO kept zero. Stores reach ONLY DMDATA0/DMDATA1, never Flash/RAM/MMIO.
// x10/x11 are fixed to verified debug-data addresses before any execution.
static const uint32_t read_program[3] = {
    0x40044180U, // c.lw x8,0(x11); c.lw x9,0(x8)
    0xc1040411U, // c.addi x8,4; c.sw x9,0(x10)
    0x9002c180U, // c.sw x8,0(x11); c.ebreak
};
typedef struct {
    rv_bus_t *bus;
    rv_dump_report_t *report;
    uint32_t data[2], program[3], gpr[4];
    bool halted, was_running, saved, modified;
} reader_t;

static rv_result_t wait_abstract(reader_t *s) {
    uint64_t deadline = s->bus->io.now_us(s->bus->io.ctx) + 100000U;
    do {
        uint32_t cs;
        TRY(read_register(s->bus, 0x16, &cs));
        s->report->abstract_cmderr = (cs >> 8) & 7U;
        if (!(cs & (1U << 12))) return s->report->abstract_cmderr ? RV_ABSTRACT_ERROR : RV_OK;
    } while (s->bus->io.now_us(s->bus->io.ctx) < deadline);
    return RV_ABSTRACT_BUSY;
}
static rv_result_t command(reader_t *s, uint32_t cmd) {
    TRY(write_register(s->bus, 0x17, cmd));
    return wait_abstract(s);
}
static rv_result_t read_gpr(reader_t *s, unsigned n, uint32_t *out) {
    TRY(command(s, 0x00221008U + n));
    return read_register(s->bus, 0x04, out);
}
static rv_result_t write_gpr(reader_t *s, unsigned n, uint32_t value) {
    TRY(write_register(s->bus, 0x04, value));
    return command(s, 0x00231008U + n);
}
static rv_result_t verify_write(rv_bus_t *b, uint8_t reg, uint32_t value) {
    uint32_t actual;
    TRY(write_register(b, reg, value));
    TRY(read_register(b, reg, &actual));
    return actual == value ? RV_OK : RV_VERIFY;
}
static rv_result_t wait_hart(rv_bus_t *b, unsigned state) {
    uint64_t deadline = b->io.now_us(b->io.ctx) + 200000U;
    do {
        uint32_t status;
        TRY(read_register(b, 0x11, &status));
        if (((status >> 8) & 0xfU) == state) return RV_OK;
    } while (b->io.now_us(b->io.ctx) < deadline);
    return RV_STATUS_INVALID;
}
static rv_result_t check_halted(reader_t *s) {
    uint32_t status, automatic;
    TRY(read_register(s->bus, 0x11, &status));
    TRY(read_register(s->bus, 0x18, &automatic));
    if (((status >> 8) & 0xfU) != 3 || (status & 0xffU) !=
        (s->report->identity.dmstatus & 0xffU)) return RV_STATUS_INVALID;
    return automatic ? RV_UNSUPPORTED : RV_OK;
}
static rv_result_t begin_reader(reader_t *s) {
    rv_bus_t *b = s->bus;
    uint32_t automatic;
    TRY(read_register(b, 0x18, &automatic));
    if (automatic) return RV_UNSUPPORTED; // never touch DATA/PROGBUF with autoexec armed
    uint32_t cs = s->report->identity.abstractcs;
    if ((cs & 0xfU) < 2 || ((cs >> 24) & 0x1fU) < 3 ||
        !(s->report->identity.hartinfo & (1U << 16)) ||
        (s->report->identity.hartinfo & 0xfffU) != 0x380U) return RV_UNSUPPORTED;
    if (cs & 0x700U) {
        s->report->abstract_cmderr = (cs >> 8) & 7U;
        return RV_ABSTRACT_ERROR;
    }
    s->was_running = ((s->report->identity.dmstatus >> 8) & 0xfU) == 12;
    if (s->was_running) {
        // Mark ownership before writing: a partially acknowledged halt may have landed.
        s->halted = true;
        TRY(write_register(b, 0x10, 0x80000001U));
        TRY(wait_hart(b, 3));
        TRY(verify_write(b, 0x10, 1)); // clear haltreq without reset
    } else s->halted = true;
    TRY(check_halted(s));
    for (unsigned i = 0; i < 2; ++i) TRY(read_register(b, 0x04 + i, &s->data[i]));
    for (unsigned i = 0; i < 3; ++i) TRY(read_register(b, 0x20 + i, &s->program[i]));
    s->saved = true;
    // GPR reads only alter DATA0; every modified GPR is saved before writing any.
    for (unsigned i = 0; i < 4; ++i) TRY(read_gpr(s, i, &s->gpr[i]));
    s->modified = true;
    TRY(write_gpr(s, 2, 0xe0000380U));
    TRY(write_gpr(s, 3, 0xe0000384U));
    for (unsigned i = 2; i < 4; ++i) {
        uint32_t actual;
        TRY(read_gpr(s, i, &actual));
        if (actual != 0xe0000380U + (i - 2) * 4U) return RV_VERIFY;
    }
    for (unsigned i = 0; i < 3; ++i) TRY(verify_write(b, 0x20 + i, read_program[i]));
    return RV_OK;
}
static rv_result_t load_word(reader_t *s, uint32_t *out) {
    TRY(command(s, 0x00240000U));
    return read_register(s->bus, 0x04, out);
}
static rv_result_t read_control_word(reader_t *s, uint32_t address, uint32_t *out) {
    // Only these two non-destructive reads are allowed outside the Flash range.
    if (address != 0x4002201cU && address != 0x1ffff7e0U) return RV_ARGUMENT;
    TRY(verify_write(s->bus, 0x05, address));
    return load_word(s, out);
}
static rv_result_t transfer_flash(reader_t *s, rv_chunk_sink_t sink, void *ctx) {
    TRY(read_control_word(s, 0x4002201cU, &s->report->obr));
    if (s->report->obr & 2U) return RV_READ_PROTECTED;
    if (s->report->obr & 1U) return RV_OPTION_ERROR;
    uint32_t capacity;
    TRY(read_control_word(s, 0x1ffff7e0U, &capacity));
    s->report->flash_kib = capacity & 0xffffU;
    if (s->report->flash_kib < 256 || s->report->flash_kib > 512) return RV_CAPACITY;
    for (unsigned pass = 0; pass < 2; ++pass) {
        // An independent target read, never replay of a buffered first pass.
        TRY(verify_write(s->bus, 0x05, RV_FLASH_BASE));
        for (uint32_t offset = 0; offset < RV_FLASH_BYTES; offset += RV_CHUNK_BYTES) {
            TRY(check_halted(s));
            uint32_t pointer;
            TRY(read_register(s->bus, 0x05, &pointer));
            if (pointer != RV_FLASH_BASE + offset) return RV_VERIFY;
            uint8_t data[RV_CHUNK_BYTES];
            for (unsigned i = 0; i < sizeof(data); i += 4) {
                uint32_t value;
                TRY(load_word(s, &value));
                for (unsigned j = 0; j < 4; ++j) data[i + j] = value >> (8 * j);
            }
            s->report->bytes[pass] += sizeof(data);
            if (!sink(ctx, pass + 1, offset, data, sizeof(data))) return RV_HOST_TRANSFER;
        }
        // Check the terminal pointer without executing a load past 0x0803ffff.
        uint32_t end;
        TRY(read_register(s->bus, 0x05, &end));
        if (end != RV_FLASH_BASE + RV_FLASH_BYTES) return RV_VERIFY;
    }
    return RV_OK;
}
static bool transport_lost(rv_result_t r) {
    return r == RV_IO || r == RV_LINE_LOW || r == RV_TIMEOUT ||
           r == RV_PARITY || r == RV_NO_RESPONSE;
}
static rv_result_t end_reader(reader_t *s, rv_result_t operation) {
    if (!s->halted) return RV_OK;
    // Do not send more writes on an untrustworthy link. Report the halted state.
    if (transport_lost(operation)) return RV_CLEANUP_FAILED;
    TRY(check_halted(s));
    uint32_t cs;
    TRY(read_register(s->bus, 0x16, &cs));
    if (cs & (1U << 12)) return RV_ABSTRACT_BUSY;
    if (cs & 0x700U) {
        TRY(write_register(s->bus, 0x16, 0x700U)); // cleanup, never retry the failed load
        TRY(wait_abstract(s));
    }
    if (s->modified) {
        for (unsigned i = 0; i < 4; ++i) {
            TRY(write_gpr(s, i, s->gpr[i]));
            uint32_t actual;
            TRY(read_gpr(s, i, &actual));
            if (actual != s->gpr[i]) return RV_VERIFY;
        }
        for (unsigned i = 0; i < 3; ++i)
            TRY(verify_write(s->bus, 0x20 + i, s->program[i]));
    }
    if (s->saved) {
        TRY(verify_write(s->bus, 0x05, s->data[1]));
        TRY(verify_write(s->bus, 0x04, s->data[0]));
    }
    s->report->restored = true;
    if (s->was_running) {
        TRY(write_register(s->bus, 0x10, 0x40000001U));
        TRY(wait_hart(s->bus, 12));
        TRY(verify_write(s->bus, 0x10, 1));
        s->report->resumed = true;
    }
    return RV_OK;
}
rv_result_t rv_dump_flash(rv_bus_t *b, unsigned mv, bool board, bool wiring,
                          rv_chunk_sink_t sink, void *ctx, rv_dump_report_t *out) {
    if (!b || !out || !sink || !b->io.release) return RV_ARGUMENT;
    memset(out, 0, sizeof(*out));
    reader_t s = {.bus = b, .report = out};
    rv_result_t r = prepare(b, mv, board, wiring);
    if (r == RV_OK) r = probe(b, &out->identity, true);
    b->phase = "flash_reader";
    b->sample = 0;
    if (r == RV_OK) r = begin_reader(&s);
    if (r == RV_OK) r = transfer_flash(&s, sink, ctx);
    out->operation_result = r;
    uint8_t cmderr = out->abstract_cmderr;
    out->cleanup_result = end_reader(&s, r);
    if (cmderr) out->abstract_cmderr = cmderr;
    out->pins_released = rv_release(b) == RV_OK;
    if (!out->pins_released) out->cleanup_result = RV_IO;
    if (r == RV_OK) r = out->cleanup_result;
    return r;
}
uint32_t rv_crc32(const uint8_t *data, unsigned length) {
    uint32_t crc = UINT32_MAX;
    for (unsigned i = 0; i < length; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
    }
    return ~crc;
}
#endif
