// SPDX-License-Identifier: MIT
// Electrical peer plus fault injection. All memory/register values are synthetic.
#include "rvswd_diag.h"
#include "rvswd_internal.h"
#include "rvswd_memory_lab.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    bool clk, dio, enabled, frame, bits[53], writing, release_fail;
    bool ignore_halt, ignore_resume, abstract_stuck, bus_stuck, release_after_activation;
    unsigned clocks, reg, reads, writes, commands, loads, halts, resumes, control_writes;
    unsigned fail_read, fail_io, io_calls, command_error, load_error;
    unsigned halt_start, halt_end, resume_start, resume_end, last_io;
    uint64_t us;
    uint32_t reply, control, status, cs, automatic, data, sbcs, address, sbdata;
    uint32_t apb1, apb2, data1;
    bool lab, lab_running, mismatch, change_state, no_resume_ack;
    bool prog, change_pc;
    uint32_t program[2], pg_gpr[2], pc;
    unsigned executions;
} model_t;
static bool parity(uint32_t v) {
    bool p = false;
    for (unsigned i = 0; i < 32; ++i) p ^= (v >> i) & 1U;
    return p;
}
static uint32_t read_dmi(model_t *m, unsigned reg) {
    ++m->reads;
    switch (reg) {
    case 0x7f: return 0x30310524U;
    case 0x10: return m->control; // W/W1 request bits read back as zero
    case 0x11: return m->status;
    case 0x12: return 0x00312380U;
    case 0x16: return m->cs;
    case 0x18: return m->automatic;
    case 0x7c: return 1;
    case 0x20: case 0x21: assert(m->prog); return m->program[reg - 0x20];
    case 0x04: assert(!m->automatic && !(m->cs & 0x1000U)); return m->data;
    case 0x05: assert(m->lab && !m->automatic && !(m->cs & 0x1000U)); return m->data1;
    case 0x38: return m->sbcs;
    case 0x3c:
        assert(!(m->sbcs & 0x0021f000U)); // no busy, errors, autoread or increment
        return m->sbdata;
    default: assert(!"Unexpected DMI read"); return 0;
    }
}
static uint32_t gpr(unsigned n) { return n ? 0xa0000000U + n : 0; }
static void write_dmi(model_t *m, unsigned reg, uint32_t v) {
    ++m->writes;
    switch (reg) {
    case 0x10:
        assert(v == 1 || v == 0x80000001U || v == 0x40000001U);
        ++m->control_writes;
        m->control = 1;
        if (v == 1 && m->release_after_activation) m->release_fail = true;
        if (v == 0x80000001U) { ++m->halts; if (!m->ignore_halt) m->status = 0x382; }
        if (v == 0x40000001U) { ++m->resumes; if (!m->ignore_resume) m->status = m->no_resume_ack ? 0xc82 : 0x30c82; }
        return;
    case 0x16:
        assert(m->lab && v == 0x700U && !(m->cs & 0x1000U));
        m->cs &= ~0x700U; return;
    case 0x04:
        assert(!m->automatic && !(m->cs & 0x1000U));
        assert(m->prog || v == 0xfeedbeefU); // DATA0 scratch only in explicit progbuf experiment
        m->data = v; return;
    case 0x05:
        assert(m->lab && !m->automatic && !(m->cs & 0x1000U));
        assert(v == 0xdeadbeefU || (v >= 0x08006400U && v < 0x08006410U) ||
               (v >= 0x08000000U && v < 0x08000010U));
        m->data1 = v; return;
    case 0x20: case 0x21:
        assert(m->prog && !m->automatic && !(m->cs & 0x1000U));
        m->program[reg - 0x20] = v; return;
    case 0x17:
        assert((m->status & 0xf00U) == (m->lab_running ? 0xc00U : 0x300U) &&
               !m->automatic && !(m->cs & 0x1700U));
        assert(v == 0x002207b1U || (v >= 0x00221000U && v <= 0x0022101fU) ||
               (m->lab && v == 0x02200000U) ||
               (m->prog && (v == 0x00231008U || v == 0x00231009U || v == 0x00240000U)));
        ++m->commands;
        if (m->prog) {
            if (v == 0x002207b1U) m->data = m->pc;
            else if (v == 0x00221008U || v == 0x00221009U) m->data = m->pg_gpr[v - 0x00221008U];
            else if (v == 0x00231008U || v == 0x00231009U) m->pg_gpr[v - 0x00231008U] = m->data;
            else {
                assert(v == 0x00240000U && m->program[0] == 0x00042483U && m->program[1] == 0x00100073U);
                uint32_t a = m->pg_gpr[0];
                bool allowed = false;
                for (unsigned i = 0; i < RV_LAB_WORDS; ++i) if (a == rv_memory_lab_address(i)) allowed = true;
                for (unsigned profile = 1; profile <= RV_LAB_PROFILE_MAX; ++profile)
                    if (a == rv_lab_address(profile, 0)) allowed = true;
                assert(allowed); // no arbitrary addresses or target stores
                ++m->executions;
                if (m->command_error) { m->cs |= m->command_error << 8; return; }
                if (m->abstract_stuck) { m->cs |= 0x1000U; return; }
                m->pg_gpr[1] = a ^ 0xa57b39efU;
                if (m->change_pc) m->pc += 4;
            }
            return;
        }
        if (m->command_error) { m->cs |= m->command_error << 8; return; }
        if (m->abstract_stuck) { m->cs |= 0x1000U; return; }
        if (m->lab) {
            m->data = m->data1 ^ 0xa57b39efU; // synthetic only
            if (m->mismatch && m->commands > RV_LAB_WORDS) m->data ^= 1;
            if (m->change_state) m->status = m->lab_running ? 0x382 : 0xc82;
        } else m->data = v == 0x002207b1U ? 0x08006438U : gpr(v - 0x00221000U);
        return;
    case 0x38:
        assert(!(m->sbcs & 0x00200000U));
        assert(!(v & ~0x001e0000U)); // never W1C or enable data autoread/increment
        m->sbcs = (m->sbcs & ~0x001f8000U) | v;
        return;
    case 0x39:
        assert(rv_snapshot_address_allowed(v));
        assert((m->sbcs & 0x001f8000U) == 0x00140000U ||
               (m->sbcs & 0x001f8000U) == 0x00120000U);
        assert(!(m->sbcs & 0x00607000U));
        ++m->loads; m->address = v;
        if (m->bus_stuck) { m->sbcs |= 0x00200000U; return; }
        if (m->load_error) { m->sbcs |= m->load_error << 12; return; }
        if (v == 0x40021018U) m->sbdata = m->apb2;
        else if (v == 0x4002101cU) m->sbdata = m->apb1;
        else m->sbdata = v ^ 0xa57b39efU;
        return;
    default: assert(!"Forbidden write: target memory, progbuf, RDP, reset or error clear");
    }
}
static bool enable(void *ctx) {
    model_t *m = ctx;
    m->enabled = true; m->clk = false; m->dio = true;
    return true;
}
static bool release(void *ctx) {
    model_t *m = ctx;
    m->enabled = false; m->frame = false; m->clk = m->dio = true;
    return !m->release_fail;
}
static bool set(void *ctx, unsigned line, bool high) {
    model_t *m = ctx;
    assert(m->enabled);
    if (++m->io_calls == m->fail_io) return false;
    if (line == RV_SWCLK) {
        if (high && !m->clk && m->frame) {
            assert(m->clocks < 53);
            m->bits[m->clocks++] = m->dio;
            if (m->clocks == 9) {
                m->reg = 0;
                for (unsigned i = 0; i < 7; ++i) m->reg = (m->reg << 1) | m->bits[i];
                m->writing = m->bits[7];
                assert(m->bits[8] == (parity(m->reg) ^ m->writing));
                if (!m->writing) m->reply = read_dmi(m, m->reg);
            }
        }
        m->clk = high;
    } else {
        if (m->clk && high != m->dio) {
            if (!high) { assert(!m->frame); m->frame = true; m->clocks = 0; }
            else if (m->frame) {
                assert(m->clocks == 53);
                for (unsigned i = 9; i < 12; ++i) assert(m->bits[i]);
                for (unsigned i = 47; i < 50; ++i) assert(m->bits[i]);
                assert(!m->bits[12] && !m->bits[13]);
                assert(m->bits[50] && !m->bits[51] && !m->bits[52]);
                if (m->writing) {
                    uint32_t value = 0;
                    for (unsigned i = 14; i < 46; ++i) value = (value << 1) | m->bits[i];
                    assert(m->bits[46] == parity(value));
                    write_dmi(m, m->reg, value);
                } else {
                    for (unsigned i = 14; i < 47; ++i) assert(m->bits[i]);
                }
                m->frame = false;
            }
        }
        m->dio = high;
    }
    return true;
}
static bool get(void *ctx, unsigned line, bool *high) {
    model_t *m = ctx;
    *high = true;
    if (line == RV_SWCLK || !m->frame) return true;
    assert(m->dio); // host must release SWDIO for every sampled target bit
    if (!m->writing && m->clocks >= 15 && m->clocks <= 46)
        *high = (m->reply >> (46 - m->clocks)) & 1U;
    else if (!m->writing && m->clocks == 47) {
        *high = parity(m->reply);
        if (m->reads == m->fail_read) *high = !*high;
    } else *high = false;
    return true;
}
static void delay(void *ctx, uint32_t us) { ((model_t *)ctx)->us += us; }
static uint64_t now(void *ctx) { return ((model_t *)ctx)->us; }
static void track(void *ctx, const rv_trace_t *trace) {
    model_t *m = ctx;
    if (trace->write && trace->reg == 0x10 && trace->word == 0x80000001U) {
        m->halt_start = m->last_io + 1; m->halt_end = m->io_calls;
    }
    if (trace->write && trace->reg == 0x10 && trace->word == 0x40000001U) {
        m->resume_start = m->last_io + 1; m->resume_end = m->io_calls;
    }
    m->last_io = m->io_calls;
}
static rv_bus_t setup(model_t *m) {
    memset(m, 0, sizeof(*m));
    m->status = 0xc82; m->control = 1; m->cs = 0x08000002U;
    m->data = 0xfeedbeefU;
    m->data1 = 0xdeadbeefU;
    m->program[0] = 0xdeadbeefU; m->program[1] = 0xfacade00U;
    m->pg_gpr[0] = 0x12345678U; m->pg_gpr[1] = 0xabcdef01U; m->pc = 0x08006700U;
    m->sbcs = 0x20040406U; // version=1, access=32, address width=32, 32-bit support
    m->apb1 = 0x0006c000U; m->apb2 = 0x0000501cU;
    rv_io_t io = {.ctx = m, .enable = enable, .release = release, .set = set,
                 .get = get, .delay_us = delay, .now_us = now};
    rv_config_t cfg = {.half_us = 250, .turnaround_us = 50, .samples = 3};
    rv_bus_t bus;
    assert(rv_init(&bus, &io, &cfg) == RV_OK);
    bus.trace_sink = track; bus.trace_ctx = m;
    return bus;
}
static rv_result_t run(rv_bus_t *b, unsigned select, bool cycle, bool gate, rv_diag_report_t *r) {
    rv_diag_options_t opts = {.select = select, .halt_cycle = cycle, .halt_resume_enabled = gate};
    return rv_capabilities(b, 3300, true, true, &opts, r);
}
static void released(model_t *m, rv_diag_report_t *r) {
    assert(!m->enabled && r->pins_released && !r->halt_requested);
    assert(!m->halts && !m->resumes);
}
int main(void) {
    model_t m;
    rv_diag_report_t r;
    rv_identity_t identity;
    rv_memory_lab_report_t lab;
    rv_bus_t lb = setup(&m);
    assert(rv_memory_lab(&lb, 3300, true, true, false, &lab) == RV_POLICY);
    assert(!m.writes && !m.reads && lab.pins_released);
#ifndef RV_ABSTRACT_MEMORY_LAB
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_POLICY);
    assert(!m.writes && !m.reads);
    assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_POLICY);
    assert(!m.writes && !m.reads);
#else
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_HART_RUNNING);
    assert(!m.writes && !m.halts && !m.resumes && lab.pins_released);
    lb = setup(&m); m.lab = true; m.status = 0x382;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_OK);
    assert(lab.command_attempts == 16 && lab.completed[0] == 8 && lab.completed[1] == 8);
    assert(lab.data_restored && lab.final_valid && !lab.state_uncertain && lab.pins_released);
    assert(!m.halts && !m.resumes && !m.control_writes && !m.loads && !lb.abstract_memory_lab);
    assert(m.data == 0xfeedbeefU && m.data1 == 0xdeadbeefU);
    for (unsigned i = 0; i < RV_LAB_WORDS; ++i)
        assert(lab.words[0][i] == (rv_memory_lab_address(i) ^ 0xa57b39efU));
    unsigned lab_reads = m.reads, lab_io = m.io_calls;
    for (unsigned n = 1; n <= lab_reads; ++n) {
        lb = setup(&m); m.lab = true; m.status = 0x382; m.fail_read = n;
        assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) != RV_OK);
        assert(lab.pins_released && !m.enabled && !m.control_writes && !lb.abstract_memory_lab);
    }
    for (unsigned n = 1; n <= lab_io; ++n) {
        lb = setup(&m); m.lab = true; m.status = 0x382; m.fail_io = n;
        assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) != RV_OK);
        assert(lab.pins_released && !m.enabled && !m.control_writes && !lb.abstract_memory_lab);
    }
    for (unsigned error = 1; error <= 7; ++error) {
        lb = setup(&m); m.lab = true; m.status = 0x382; m.command_error = error;
        assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_ABSTRACT_ERROR);
        assert(lab.cmderr == error && !lab.completed[0] && lab.command_attempts == 1);
        assert(lab.data_restored && ((m.cs >> 8) & 7U) == error);
    }
    lb = setup(&m); m.lab = true; m.status = 0x382; m.abstract_stuck = true;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_CLEANUP_FAILED);
    assert(lab.operation_result == RV_POLL_TIMEOUT && lab.state_uncertain && lab.pins_released);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.mismatch = true;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_VERIFY);
    assert(lab.completed[1] == 1 && lab.data_restored);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.change_state = true;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_CLEANUP_FAILED);
    assert(lab.state_uncertain && lab.command_attempts == 1 && !m.control_writes);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.automatic = 1;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_AUTOEXEC);
    assert(!m.writes);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.cs |= 0x200;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_ABSTRACT_ERROR);
    assert(!m.writes && lab.cmderr == 2);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.control = 0;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_DM_INACTIVE);
    assert(!m.writes);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.cs = 0x08000001U;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_UNSUPPORTED);
    assert(!m.writes);
    lb = setup(&m); m.lab = true; m.status = 0x382; m.release_fail = true;
    assert(rv_memory_lab(&lb, 3300, true, true, true, &lab) == RV_CLEANUP_FAILED);
    assert(!lab.pins_released && lab.cleanup_result == RV_IO);
    lb = setup(&m);
    assert(rv_memory_lab(&lb, 2900, true, true, true, &lab) == RV_VOLTAGE);
    assert(!m.writes && !m.reads);
    assert(rv_memory_lab(&lb, 3300, false, true, true, &lab) == RV_BOARD_UNVERIFIED);
    assert(rv_memory_lab(&lb, 3300, true, false, true, &lab) == RV_WIRING_UNCHECKED);
    lb = setup(&m); m.lab = m.lab_running = true;
    assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_OK);
    assert(lab.final_status == 0xc82 && lab.completed[1] == 8 && lab.data_restored);
    assert(!m.halts && !m.resumes && !m.control_writes);
    unsigned running_reads = m.reads, running_io = m.io_calls;
    for (unsigned n = 1; n <= running_reads; ++n) {
        lb = setup(&m); m.lab = m.lab_running = true; m.fail_read = n;
        assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) != RV_OK);
        assert(lab.pins_released && !m.control_writes && !lb.abstract_memory_lab);
    }
    for (unsigned n = 1; n <= running_io; ++n) {
        lb = setup(&m); m.lab = m.lab_running = true; m.fail_io = n;
        assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) != RV_OK);
        assert(lab.pins_released && !m.control_writes && !lb.abstract_memory_lab);
    }
    for (unsigned error = 1; error <= 7; ++error) {
        lb = setup(&m); m.lab = m.lab_running = true; m.command_error = error;
        assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_ABSTRACT_ERROR);
        assert(lab.cmderr == error && lab.command_attempts == 1 && lab.data_restored);
        assert(!m.control_writes);
    }
    lb = setup(&m); m.lab = m.lab_running = true; m.change_state = true;
    assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_CLEANUP_FAILED);
    assert(lab.state_uncertain && !m.control_writes);
    lb = setup(&m); m.lab = m.lab_running = true; m.abstract_stuck = true;
    assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_CLEANUP_FAILED);
    assert(lab.operation_result == RV_POLL_TIMEOUT && lab.pins_released);
    lb = setup(&m); m.lab = m.lab_running = true; m.status = 0x382;
    assert(rv_memory_lab_running_spare(&lb, 3300, true, true, true, &lab) == RV_STATUS_INVALID);
    assert(!m.writes);
    puts("PASS: lab memory guards, 16 independent synthetic reads, errors 1..7, busy, mismatch, state change, every GPIO/read failure");
    { // Explicit owned halt/read/resume cycle; all data remain synthetic.
        rv_diag_report_t cr;
        rv_diag_options_t co = {.halt_cycle = true, .halt_resume_enabled = true, .memory_lab_cycle = true};
        lb = setup(&m); m.lab = true; m.cs |= 0x400U;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_OK);
        assert(cr.cmderr_cleared && cr.initial_cmderr == 4 && cr.halt_observed && cr.resume_observed);
        assert(cr.memory_lab.completed[1] == 8 && cr.memory_lab.data_restored && cr.pins_released);
        assert(!cr.target_state_uncertain && m.halts == 1 && m.resumes == 1 && !m.loads);
        unsigned reads = m.reads, ios = m.io_calls;
        lb = setup(&m); m.lab = true; m.no_resume_ack = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_OK);
        assert(cr.resume_observed && !cr.resume_ack_observed && !cr.target_state_uncertain);
        for (unsigned n = 1; n <= reads; ++n) {
            lb = setup(&m); m.lab = true; m.cs |= 0x400U; m.fail_read = n;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) != RV_OK);
            assert(cr.pins_released && !m.enabled && m.halts <= 1 && m.resumes <= 1);
        }
        for (unsigned n = 1; n <= ios; ++n) {
            lb = setup(&m); m.lab = true; m.cs |= 0x400U; m.fail_io = n;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) != RV_OK);
            assert(cr.pins_released && !m.enabled && m.halts <= 1 && m.resumes <= 1);
        }
        for (unsigned e = 1; e <= 7; ++e) {
            lb = setup(&m); m.lab = true; m.cs |= 0x400U; m.command_error = e;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_ABSTRACT_ERROR);
            assert(cr.memory_lab.cmderr == e && cr.memory_lab.command_attempts == 1);
            assert(cr.resume_observed && cr.memory_lab.data_restored && !cr.target_state_uncertain);
        }
        lb = setup(&m); m.lab = true; m.ignore_halt = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_POLL_TIMEOUT);
        assert(cr.resume_observed && !m.commands && !cr.target_state_uncertain);
        lb = setup(&m); m.lab = true; m.ignore_resume = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_CLEANUP_FAILED);
        assert(cr.target_state_uncertain && cr.pins_released);
        lb = setup(&m); m.lab = true; m.abstract_stuck = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_CLEANUP_FAILED);
        assert(cr.target_state_uncertain && !m.resumes && cr.pins_released);
        lb = setup(&m); m.lab = true; co.halt_resume_enabled = false;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_POLICY);
        assert(!m.writes && cr.pins_released);
    }

#endif
#ifdef RV_PROGBUF_LAB
    {
        rv_diag_report_t cr;
        rv_diag_options_t co = {.halt_cycle = true, .halt_resume_enabled = true,
                                .memory_lab_cycle = true, .progbuf_lab_cycle = true};
        lb = setup(&m); m.lab = m.prog = m.no_resume_ack = true; m.cs |= 0x300;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_OK);
        assert(m.executions == 16 && cr.memory_lab.completed[1] == 8);
        assert(cr.memory_lab.gprs_restored && cr.memory_lab.program_restored && cr.memory_lab.pc_unchanged);
        assert(m.pg_gpr[0] == 0x12345678U && m.pg_gpr[1] == 0xabcdef01U && m.data == 0xfeedbeefU);
        assert(m.program[0] == 0xdeadbeefU && m.program[1] == 0xfacade00U && m.pc == 0x08006700U);
        assert(cr.resume_observed && !cr.target_state_uncertain && !m.loads);
        unsigned reads = m.reads, ios = m.io_calls;
        for (unsigned profile = 1; profile <= RV_LAB_PROFILE_MAX; ++profile) {
            lb = setup(&m); m.lab = m.prog = m.no_resume_ack = true; co.memory_profile = profile;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_OK);
            assert(m.executions == 2 && cr.memory_lab.completed[1] == 1 && cr.memory_lab.profile == profile);
            assert(cr.memory_lab.words[0][0] == (rv_lab_address(profile, 0) ^ 0xa57b39efU));
            assert(cr.resume_observed && cr.memory_lab.pc_unchanged && cr.memory_lab.gprs_restored);
        }
        co.memory_profile = 5; lb = setup(&m);
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_POLICY && !m.writes);
        co.memory_profile = 0;

        for (unsigned n = 1; n <= reads; ++n) {
            lb = setup(&m); m.lab = m.prog = m.no_resume_ack = true; m.cs |= 0x300U; m.fail_read = n;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) != RV_OK);
            assert(cr.pins_released && !m.enabled && m.halts <= 1 && m.resumes <= 1);
        }
        // Representative partial GPIO frames over every phase, plus final edges.
        for (unsigned n = 1; n <= ios; n += 53) {
            lb = setup(&m); m.lab = m.prog = m.no_resume_ack = true; m.cs |= 0x300U; m.fail_io = n;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) != RV_OK);
            assert(cr.pins_released && !m.enabled && m.halts <= 1 && m.resumes <= 1);
        }
        for (unsigned err = 1; err <= 7; ++err) {
            lb = setup(&m); m.lab = m.prog = true; m.command_error = err;
            assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_ABSTRACT_ERROR);
            assert(cr.memory_lab.cmderr == err && m.executions == 1 && cr.resume_observed);
            assert(cr.memory_lab.gprs_restored && cr.memory_lab.program_restored && cr.memory_lab.pc_unchanged);
        }
        lb = setup(&m); m.lab = m.prog = m.change_pc = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_CLEANUP_FAILED);
        assert(!m.resumes && cr.target_state_uncertain && cr.pins_released);
        lb = setup(&m); m.lab = m.prog = m.abstract_stuck = true;
        assert(rv_capabilities(&lb, 3300, true, true, &co, &cr) == RV_CLEANUP_FAILED);
        assert(!m.resumes && cr.target_state_uncertain && cr.pins_released);
        puts("PASS: fixed lw/ebreak progbuf, saved GPR/PC/program/DATA, all read faults, partial GPIO faults, cmderr 1..7, busy and changed-PC no-resume");
    }
#endif
    rv_bus_t activation_bus = setup(&m);
    m.control = 0;
    assert(rv_activate_dm(&activation_bus, 3300, true, true, &identity) == RV_OK);
    assert(identity.dmactive_write_attempted && identity.dmactive_set);
    assert(identity.dmcontrol_readback_valid && identity.dmstatus_readback_valid);
    assert(!identity.dmactive_already_active && !identity.activation_state_uncertain);
    assert(identity.dmcontrol == 1 && identity.dmstatus == 0xc82 && identity.pins_released);
    assert(m.writes == 1 && m.control_writes == 1 && !m.halts && !m.resumes && !m.commands && !m.loads);
    assert(!m.enabled);
    activation_bus = setup(&m);
    assert(rv_activate_dm(&activation_bus, 3300, true, true, &identity) == RV_OK);
    assert(identity.dmactive_already_active && !identity.dmactive_write_attempted);
    assert(identity.dmcontrol_readback_valid && identity.dmstatus_readback_valid);
    assert(identity.pins_released && m.writes == 0 && !m.enabled);

    activation_bus = setup(&m);
    m.control = 0;
    m.release_after_activation = true;
    assert(rv_activate_dm(&activation_bus, 3300, true, true, &identity) == RV_CLEANUP_FAILED);
    assert(m.writes == 1 && identity.dmactive_write_attempted && !identity.pins_released);

    rv_bus_t b = setup(&m);
    assert(run(&b, 0, false, false, &r) == RV_OK);
    assert(m.writes == 0 && !m.commands && !m.loads);
    assert(r.capabilities[RV_CAP_DMI].state == RV_CAP_SUPPORTED);
    assert(r.capabilities[RV_CAP_HALT].state == RV_CAP_UNKNOWN);
    assert(r.abstractauto_valid && r.sbcs_valid && r.final_status_valid);
    released(&m, &r);
    unsigned status_reads = m.reads;
    unsigned status_calls = m.io_calls;
    for (unsigned n = 1; n <= status_reads; ++n) {
        b = setup(&m); m.fail_read = n;
        rv_result_t result = run(&b, 0, false, false, &r);
        assert(result == RV_PARITY || result == RV_CLEANUP_FAILED);
        assert(!m.writes); released(&m, &r);
    }
    for (unsigned n = 1; n <= status_calls; ++n) {
        b = setup(&m); m.fail_io = n;
        rv_result_t result = run(&b, 0, false, false, &r);
        assert(result == RV_IO || result == RV_CLEANUP_FAILED);
        assert(!m.writes); released(&m, &r);
    }
    b = setup(&m); m.control = 0;
    assert(run(&b, 0, false, false, &r) == RV_DM_INACTIVE);
    assert(!m.writes); released(&m, &r);
    b = setup(&m); m.status &= ~0x80U;
    assert(run(&b, RV_DIAG_MASK, false, false, &r) == RV_AUTH_REQUIRED);
    assert(!m.writes && !m.loads && r.capabilities[RV_CAP_SRAM].state == RV_CAP_BLOCKED);
    b = setup(&m);
    assert(run(&b, 0, true, false, &r) == RV_POLICY);
    assert(!m.reads && !m.writes && !m.enabled);
    b = setup(&m);
    assert(run(&b, 32, false, false, &r) == RV_ARGUMENT);
    assert(!m.reads && !m.writes);
    b = setup(&m); m.status = 0x382;
    assert(run(&b, 0, true, true, &r) == RV_STATUS_INVALID);
    assert(!m.writes && !m.halts && !m.resumes); // externally halted stays halted
    b = setup(&m);
    assert(run(&b, 1, false, false, &r) == RV_HART_RUNNING);
    assert(!m.writes && !m.commands); released(&m, &r);
    b = setup(&m);
    assert(run(&b, 30, false, false, &r) == RV_OK);
    assert(m.loads == 48 && m.status == 0xc82 && !m.commands);
    released(&m, &r); // snapshots work without changing a running hart
    b = setup(&m); m.status = 0x382; m.automatic = 1;
    assert(run(&b, 1, false, false, &r) == RV_AUTOEXEC);
    assert(!m.writes && !m.commands); released(&m, &r);
    b = setup(&m); m.sbcs = 0;
    assert(run(&b, 2, false, false, &r) == RV_UNSUPPORTED);
    assert(!m.writes && !m.loads && r.capabilities[RV_CAP_SRAM].state == RV_CAP_BLOCKED);
    b = setup(&m); m.sbcs |= 0x8000;
    assert(run(&b, 2, false, false, &r) == RV_AUTOEXEC && !m.writes);
    b = setup(&m); m.status = 0x382;
    assert(run(&b, RV_DIAG_MASK, false, false, &r) == RV_OK);
    assert(m.commands == 33 && m.data == 0xfeedbeefU && r.data0_restored);
    assert(r.pc_valid && r.pc == 0x08006438U && r.gpr_valid_mask == UINT32_MAX);
    for (unsigned i = 0; i < 32; ++i) assert(r.gpr[i] == gpr(i));
    assert(r.word_count == 48 && m.loads == 48 && r.sba_config_restored && m.sbcs == 0x20040406U);
    for (unsigned i = 0; i < r.word_count; ++i) assert(rv_snapshot_address_allowed(r.words[i].address));
    released(&m, &r);
    unsigned full_reads = m.reads;
    // Every DMI read in a successful register+memory session can lose parity.
    for (unsigned n = 1; n <= full_reads; ++n) {
        b = setup(&m); m.status = 0x382; m.fail_read = n;
        rv_result_t result = run(&b, RV_DIAG_MASK, false, false, &r);
        assert(result == RV_PARITY || result == RV_CLEANUP_FAILED);
        assert(!m.enabled && r.pins_released && !m.halts && !m.resumes);
    }
    b = setup(&m); m.sbcs &= ~2U;
    assert(run(&b, RV_DIAG_SPI, false, false, &r) == RV_UNSUPPORTED);
    assert(m.loads == 2 && r.capabilities[RV_CAP_SPI].state == RV_CAP_BLOCKED);
    b = setup(&m); m.apb1 = m.apb2 = 0;
    assert(run(&b, 28, false, false, &r) == RV_OK);
    assert(m.loads == 2 && r.capabilities[RV_CAP_GPIO].reason == RV_CLOCK_DISABLED);
    for (unsigned error = 1; error <= 7; ++error) {
        b = setup(&m); m.load_error = error;
        rv_result_t expected = error == 1 ? RV_POLL_TIMEOUT : RV_BUS_ERROR;
        assert(run(&b, 2, false, false, &r) == expected);
        assert(m.loads == 1 && r.word_count == 0 && r.sberror == error);
        assert(r.sba_config_restored && ((m.sbcs >> 12) & 7U) == error);
        assert(r.capabilities[RV_CAP_SRAM].state == RV_CAP_UNKNOWN); // not proof of RDP
    }
    b = setup(&m); m.bus_stuck = true;
    assert(run(&b, 2, false, false, &r) == RV_CLEANUP_FAILED);
    assert(r.operation_result == RV_POLL_TIMEOUT && !r.sba_config_restored && m.loads == 1);
    for (unsigned error = 2; error <= 5; ++error) {
        b = setup(&m); m.status = 0x382; m.command_error = error;
        assert(run(&b, 1, false, false, &r) == RV_ABSTRACT_ERROR);
        assert(r.abstract_cmderr == error && r.data0_restored && m.commands == 1);
        assert(((m.cs >> 8) & 7U) == error); // preserve sticky evidence, no clearing
        assert(r.capabilities[RV_CAP_PC].state == (error == 2 ? RV_CAP_BLOCKED : RV_CAP_UNKNOWN));
    }
    b = setup(&m); m.status = 0x382; m.abstract_stuck = true;
    assert(run(&b, 1, false, false, &r) == RV_CLEANUP_FAILED);
    assert(r.operation_result == RV_POLL_TIMEOUT && m.commands == 1 && !r.data0_restored);
    b = setup(&m);
    assert(run(&b, 1, true, true, &r) == RV_OK);
    assert(m.halts == 1 && m.resumes == 1 && r.halt_observed && r.resume_observed);
    assert(r.control_restored && !r.target_state_uncertain && r.data0_restored);
    assert(r.final_dmstatus == 0x30c82 && r.final_dmcontrol == 1 && !m.enabled);
    unsigned cycle_reads = m.reads;
    unsigned ranges[] = {m.halt_start, m.halt_end, m.resume_start, m.resume_end};
    unsigned write_faults = 0;
    for (unsigned range = 0; range < 2; ++range) {
        for (unsigned n = ranges[2 * range]; n <= ranges[2 * range + 1]; ++n) {
            b = setup(&m); m.fail_io = n;
            assert(run(&b, 1, true, true, &r) == RV_CLEANUP_FAILED);
            assert(!m.enabled && r.pins_released && r.target_state_uncertain);
            assert(m.halts <= 1 && m.resumes <= 1);
            ++write_faults;
        }
    }
    for (unsigned n = 1; n <= cycle_reads; ++n) {
        b = setup(&m); m.fail_read = n;
        rv_result_t result = run(&b, 1, true, true, &r);
        assert(result == RV_PARITY || result == RV_CLEANUP_FAILED);
        assert(!m.enabled && r.pins_released && m.halts <= 1 && m.resumes <= 1);
        if (r.halt_requested) assert(r.target_state_uncertain);
    }
    b = setup(&m); m.ignore_halt = true;
    assert(run(&b, 0, true, true, &r) == RV_POLL_TIMEOUT);
    assert(r.operation_result == RV_POLL_TIMEOUT && r.cleanup_result == RV_OK);
    assert(r.resume_observed && !r.target_state_uncertain && m.halts == 1 && m.resumes == 1);
    b = setup(&m); m.ignore_resume = true;
    assert(run(&b, 0, true, true, &r) == RV_CLEANUP_FAILED);
    assert(r.cleanup_result == RV_POLL_TIMEOUT && r.target_state_uncertain && r.control_restored);
    b = setup(&m); m.release_fail = true;
    assert(run(&b, 0, false, false, &r) == RV_CLEANUP_FAILED);
    assert(!r.pins_released && r.cleanup_result == RV_IO);
    b = setup(&m);
    const uint8_t write_regs[] = {0x16, 0x17, 0x20, 0x3c, 0x10, 0x39};
    const uint32_t write_values[] = {0x700U, 0x00231008U, 0x9002U, 0, 3, 0x08000000U};
    for (unsigned n = 0; n < sizeof(write_regs); ++n)
        assert(rv_diag_write(&b, write_regs[n], write_values[n]) == RV_ARGUMENT);
    assert(!m.writes && !m.io_calls);
    const uint32_t forbidden[] = {0, 0x08000000U, 0x1ffff800U, 0x1ffff7e0U, 0x40022000U,
                                  0x40010810U, 0x40010818U, 0x40013800U, 0x40013804U,
                                  0x40013008U, 0x4001300cU, 0x20000040U, UINT32_MAX};
    for (unsigned i = 0; i < sizeof(forbidden) / sizeof(forbidden[0]); ++i)
        assert(!rv_snapshot_address_allowed(forbidden[i]));
    assert(!strcmp(rv_failure_class(RV_READ_PROTECTED), "rdp_blocked"));
    assert(!strcmp(rv_failure_class(RV_PARITY), "connection_error"));
    assert(!strcmp(rv_failure_class(RV_POLL_TIMEOUT), "timeout"));
    assert(!strcmp(rv_failure_class(RV_BUS_ERROR), "access_error_cause_unproven"));
    printf("PASS: diagnostics, 33 register reads, 48 safe snapshot words, gates, sticky errors, halt/resume cleanup; %u GPIO, %u partial write and %u DMI parity fault positions\n",
           status_calls, write_faults, status_reads + full_reads + cycle_reads);
    return 0;
}
