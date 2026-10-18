// SPDX-License-Identifier: MIT
// Electrical DMI peer for the fixed CH32V30x read sequence; not hardware validation.
#include "rvswd_ro.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    bool clk, dio, enabled, frame, bits[53], writing, running, corrupt;
    unsigned clocks, reg, loads[2], total_loads, executions, chunks, activations;
    uint64_t us;
    uint32_t reply, control, status, cs, data[2], program[3], gpr[4], obr, capacity;
    unsigned fail_load, parity_load, reject_gpr, reject_pointer, stop_sink;
    bool saw_clear;
    unsigned reject_activation, corrupt_control, trace_count, activation_traces;
    bool cpbr_unavailable;
} model_t;
static const uint32_t original_data[] = {0xfeedbeefU, 0x12345678U};
static const uint32_t original_program[] = {0xdeadbeefU, UINT32_MAX, 0};
static const uint32_t original_gpr[] = {0x11223344U, 0x55667788U, 0x123U, 0x456U};
static uint32_t flash_word(uint32_t address) {
    return address ^ 0xa57b39efU;
}
static bool parity(uint32_t value) {
    bool p = false;
    for (unsigned i = 0; i < 32; ++i) p ^= (value >> i) & 1U;
    return p;
}
static uint32_t read_dmi(model_t *m, unsigned reg) {
    switch (reg) {
    case 0x7f: return 0x30310524U;
    case 0x10: return m->control;
    case 0x11: return m->status;
    case 0x12: return 0x00312380U;
    case 0x16: return m->cs;
    case 0x18: return 0;
    case 0x7c: return m->cpbr_unavailable ? UINT32_MAX : 1;
    case 0x04: case 0x05: return m->data[reg - 4];
    case 0x20: case 0x21: case 0x22: return m->program[reg - 0x20];
    default: assert(!"Unexpected DMI read"); return 0;
    }
}
static void write_dmi(model_t *m, unsigned reg, uint32_t value) {
    switch (reg) {
    case 0x10:
        assert(value == 1 || value == 0x80000001U || value == 0x40000001U);
        if (!m->control && value == 1) m->activations++;
        if (m->reject_activation && !m->control && value == 1) return;
        m->control = value;
        if (value == 0x80000001U) { m->running = false; m->status = 0x382; }
        if (value == 0x40000001U) { m->running = true; m->status = 0xc82; }
        return;
    case 0x04: case 0x05:
        if (reg == 5 && value == RV_FLASH_BASE && m->reject_pointer) return;
        m->data[reg - 4] = value;
        return;
    case 0x20: case 0x21: case 0x22:
        m->program[reg - 0x20] = value;
        return;
    case 0x16:
        assert(value == 0x700U && (m->cs & 0x700U));
        m->saw_clear = true;
        m->cs &= ~0x700U;
        return;
    case 0x17:
        assert(!m->running && !(m->cs & 0x1700U));
        if (value >= 0x00221008U && value <= 0x0022100bU) {
            m->data[0] = m->gpr[value - 0x00221008U];
        } else if (value >= 0x00231008U && value <= 0x0023100bU) {
            unsigned n = value - 0x00231008U;
            if (!(m->reject_gpr && n == 2 && m->data[0] == 0xe0000380U))
                m->gpr[n] = m->data[0];
        } else {
            assert(value == 0x00240000U);
            m->executions++;
            assert(m->program[0] == 0x40044180U && m->program[1] == 0xc1040411U &&
                   m->program[2] == 0x9002c180U);
            // Every store instruction's destination is the debug DATA window.
            assert(m->gpr[2] == 0xe0000380U && m->gpr[3] == 0xe0000384U);
            uint32_t addr = m->data[1], result;
            assert(!(addr & 3U));
            if (addr == 0x4002201cU) result = m->obr;
            else if (addr == 0x1ffff7e0U) result = m->capacity;
            else {
                assert(addr >= RV_FLASH_BASE && addr < RV_FLASH_BASE + RV_FLASH_BYTES);
                unsigned p = m->total_loads / (RV_FLASH_BYTES / 4U);
                assert(p < 2);
                m->total_loads++;
                m->loads[p]++;
                if (m->total_loads == m->fail_load) { m->cs |= 3U << 8; return; }
                result = flash_word(addr);
            }
            m->gpr[0] = addr + 4;
            m->gpr[1] = result;
            m->data[0] = result;
            m->data[1] = addr + 4;
        }
        return;
    default: assert(!"Unexpected DMI write");
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
    return true;
}
static bool set(void *ctx, unsigned line, bool high) {
    model_t *m = ctx;
    assert(m->enabled);
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
        if (m->reg == 0x10 && m->corrupt_control) *high = !*high;
        if (m->reg == 0x7c && m->cpbr_unavailable) *high = !*high;
        if (m->reg == 4 && m->parity_load && m->total_loads == m->parity_load) {
            *high = !*high;
            m->corrupt = true;
        }
    } else *high = false;
    return true;
}
static void delay(void *ctx, uint32_t us) { ((model_t *)ctx)->us += us; }
static uint64_t now(void *ctx) { return ((model_t *)ctx)->us; }
static rv_bus_t setup(model_t *m) {
    memset(m, 0, sizeof(*m));
    m->status = 0xc82; m->running = true; m->cs = 0x08000002U; m->capacity = 256;
    memcpy(m->data, original_data, sizeof(m->data));
    memcpy(m->program, original_program, sizeof(m->program));
    memcpy(m->gpr, original_gpr, sizeof(m->gpr));
    rv_io_t io = {.ctx = m, .enable = enable, .release = release, .set = set,
                 .get = get, .delay_us = delay, .now_us = now};
    rv_config_t cfg = {.half_us = 250, .turnaround_us = 50, .samples = 3};
    rv_bus_t bus;
    assert(rv_init(&bus, &io, &cfg) == RV_OK);
    return bus;
}
static bool sink(void *ctx, unsigned pass, uint32_t offset, const uint8_t *data, unsigned n) {
    model_t *m = ctx;
    unsigned index = m->chunks++;
    assert(pass == index / (RV_FLASH_BYTES / RV_CHUNK_BYTES) + 1);
    assert(offset == (index % (RV_FLASH_BYTES / RV_CHUNK_BYTES)) * RV_CHUNK_BYTES);
    assert(n == RV_CHUNK_BYTES);
    for (unsigned i = 0; i < n; ++i)
        assert(data[i] == (uint8_t)(flash_word(RV_FLASH_BASE + offset + (i & ~3U)) >> (8 * (i & 3U))));
    return !m->stop_sink;
}
static void restored(model_t *m, rv_dump_report_t *r) {
    assert(!m->enabled && r->pins_released && m->running && r->resumed && r->restored);
    assert(r->cleanup_result == RV_OK && m->control == 1 && m->activations == 1);
    assert(!memcmp(m->data, original_data, sizeof(m->data)));
    assert(!memcmp(m->gpr, original_gpr, sizeof(m->gpr)));
    assert(!memcmp(m->program, original_program, sizeof(m->program)));
}
static void trace_activation(void *ctx, const rv_trace_t *t) {
    model_t *m = ctx;
    ++m->trace_count;
    if (t->write) {
        assert(t->reg == 0x10 && t->word == 1);
        assert(!strcmp(t->phase, "dmactive_write"));
        assert(t->sequence == 16); // three samples of five required registers
        ++m->activation_traces;
    }
    if (!strcmp(t->phase, "dmactive_readback")) {
        assert(t->sequence == 17 && t->reg == 0x10 && !t->write && t->word == 1);
        assert(t->result == RV_OK);
    }
    if (t->reg == 0x7c) {
        assert(t->sequence == 33 && t->optional);
        assert(!strcmp(t->phase, "optional_cpbr") && m->activation_traces == 1);
    }
}
int main(void) {
    model_t m;
    rv_dump_report_t r;
    rv_bus_t b = setup(&m);
    rv_identity_t id;
    // Inactive targets cannot trigger a hidden activation from identify or dump.
    assert(rv_identify(&b, 3300, true, true, &id) == RV_DM_INACTIVE);
    assert(m.activations == 0 && !m.total_loads && m.running && !m.enabled);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_DM_INACTIVE);
    assert(m.activations == 0 && !m.total_loads && m.running && !m.enabled);
    b.trace_sink = trace_activation; b.trace_ctx = &m;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(id.dmactive_set && id.dmcontrol == 1 && m.activation_traces == 1 && m.trace_count == 33);
    assert(m.activations == 1 && !m.total_loads && m.running && !m.enabled);
    b = setup(&m); m.cpbr_unavailable = true;
    assert(rv_diagnose(&b, 3300, true, true, &id) == RV_OK);
    assert(id.completed_samples == 3 && id.dmcontrol == 0 && id.cpbr_result == RV_PARITY);
    assert(id.cpbr_attempted && !id.cpbr_valid && !m.activations && !m.total_loads && !m.enabled);
    b.trace_sink = trace_activation; b.trace_ctx = &m;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(id.dmactive_set && id.dmcontrol == 1 && id.cpbr_result == RV_PARITY);
    assert(m.activations == 1 && m.activation_traces == 1 && m.trace_count == 33 && !m.total_loads);
    b = setup(&m); m.reject_activation = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_DM_INACTIVE);
    assert(m.activations == 1 && !id.dmactive_set && !m.total_loads && !m.enabled); // no retry
    b = setup(&m); m.corrupt_control = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_PARITY);
    assert(!m.activations && !m.total_loads && !m.enabled);
    b = setup(&m);
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_crc32((const uint8_t *)"123456789", 9) == 0xcbf43926U);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_OK);
    assert(r.bytes[0] == RV_FLASH_BYTES && r.bytes[1] == RV_FLASH_BYTES);
    assert(m.loads[0] == 65536 && m.loads[1] == 65536 && m.chunks == 4096);
    restored(&m, &r);
    b = setup(&m); m.obr = 2;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_READ_PROTECTED);
    assert(!m.total_loads && !m.chunks); restored(&m, &r);
    b = setup(&m); m.obr = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_OPTION_ERROR);
    assert(!m.total_loads); restored(&m, &r);
    b = setup(&m); m.capacity = 128;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_CAPACITY);
    assert(!m.total_loads); restored(&m, &r);
    b = setup(&m); m.fail_load = 17;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_ABSTRACT_ERROR);
    assert(m.total_loads == 17 && m.saw_clear && r.abstract_cmderr == 3);
    restored(&m, &r);
    b = setup(&m); m.stop_sink = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_HOST_TRANSFER);
    assert(m.total_loads == 32 && m.chunks == 1); restored(&m, &r);
    b = setup(&m); m.parity_load = 17;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_PARITY);
    assert(m.total_loads == 17 && m.corrupt && !m.running && !m.enabled);
    assert(r.cleanup_result == RV_CLEANUP_FAILED && !r.restored && r.pins_released);
    b = setup(&m); m.reject_gpr = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_VERIFY);
    assert(!m.executions); restored(&m, &r);
    b = setup(&m); m.reject_pointer = 1;
    assert(rv_activate_dm(&b, 3300, true, true, &id) == RV_OK);
    assert(rv_dump_flash(&b, 3300, true, true, sink, &m, &r) == RV_VERIFY);
    assert(!m.total_loads); restored(&m, &r);
    puts("PASS: two complete independent reads, debug-only stores, restoration, RDP, option/capacity checks, no retry after access/parity/host errors");
    return 0;
}
