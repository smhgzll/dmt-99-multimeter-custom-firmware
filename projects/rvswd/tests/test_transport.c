// SPDX-License-Identifier: MIT
// Scripted electrical model: validates host framing and policy, not WCH hardware.
#include "rvswd_ro.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    bool dio, clk, enabled;
    bool queue[4096], edges[8192];
    unsigned count, pos, edge_count, enable_count, releases, calls, fail_at;
    uint64_t us;
    bool stuck_clock, timeout;
    rv_trace_t trace[194];
    unsigned traces;
} model_t;
static bool fail(model_t *m) { return ++m->calls == m->fail_at; }
static bool enable(void *p) {
    model_t *m = p;
    m->enable_count++;
    m->enabled = true;
    m->clk = false;
    m->dio = true;
    return !fail(m);
}
static bool release(void *p) {
    model_t *m = p;
    m->releases++;
    m->enabled = false;
    m->dio = m->clk = true;
    return true;
}
static bool set(void *p, unsigned line, bool high) {
    model_t *m = p;
    assert(m->enabled);
    if (fail(m)) return false;
    if (line == RV_SWCLK) {
        if (high && !m->clk) {
            assert(m->edge_count < sizeof(m->edges));
            m->edges[m->edge_count++] = m->dio;
        }
        m->clk = high;
    } else m->dio = high;
    return true;
}
static bool get(void *p, unsigned line, bool *v) {
    model_t *m = p;
    if (fail(m)) return false;
    if (line == RV_SWCLK) *v = !m->stuck_clock;
    else {
        assert(m->pos < m->count);
        *v = m->queue[m->pos++];
    }
    return true;
}
static void delay(void *p, uint32_t us) {
    model_t *m = p;
    m->us += m->timeout ? 100001 : us;
}
static uint64_t now(void *p) { return ((model_t *)p)->us; }
static void append(model_t *m, bool v) { m->queue[m->count++] = v; }
static void word(model_t *m, uint32_t value, bool corrupt) {
    append(m, true); // pre-START released line check
    for (unsigned i = 0; i < 3; ++i) append(m, i & 1U); // arbitrary target aux
    bool parity = false;
    for (int i = 31; i >= 0; --i) {
        bool v = (value >> i) & 1U;
        append(m, v);
        parity ^= v;
    }
    append(m, parity ^ corrupt);
    for (unsigned i = 0; i < 3; ++i) append(m, i & 1U);
}
static rv_bus_t setup(model_t *m, int mode) {
    memset(m, 0, sizeof(*m));
    append(m, true); // synchronization line check
    for (unsigned sample = 0; sample < 3; ++sample) {
        uint32_t id = mode == 1 ? 0 : mode == 2 ? UINT32_MAX :
                      mode == 3 ? 0x30700504U : 0x30300504U;
        if (mode == 4 && sample == 1) id ^= 1;
        word(m, id, mode == 5);
        word(m, mode == 6 ? 0 : mode == 12 ? 2 : mode == 13 && sample == 0 ? 0 :
                mode == 16 ? UINT32_MAX : 1, mode == 16);
        word(m, mode == 14 && sample == 1 ? 0x382 : mode == 7 ? 0xc02 : mode == 9 ? 0xfc82 :
                mode == 10 ? 0x82 : mode == 11 ? 0xf82 : 0xc82, false);
        word(m, 0x380, false);
        word(m, mode == 15 && sample == 1 ? 0x04000002 :
                mode == 8 ? 0x08001002 : 0x08000002, false);
    }
    unsigned cpbr_start = m->count;
    word(m, mode == 17 ? UINT32_MAX : 1, mode == 17);
    if (mode == 17) {
        for (unsigned i = 1; i <= 3; ++i) m->queue[cpbr_start + i] = true;
        for (unsigned i = 37; i <= 39; ++i) m->queue[cpbr_start + i] = true;
    }
    rv_bus_t bus;
    rv_io_t io = {.ctx = m, .enable = enable, .release = release, .set = set,
                   .get = get, .delay_us = delay, .now_us = now};
    rv_config_t cfg = {.half_us = 25, .turnaround_us = 10, .samples = 3};
    assert(rv_init(&bus, &io, &cfg) == RV_OK);
    assert(!m->enabled && !m->enable_count);
    return bus;
}
static void verify_frames(model_t *m) {
    assert(m->edge_count == 102 + 16 * 54);
    const unsigned regs[] = {0x7f, 0x10, 0x11, 0x12, 0x16};
    for (unsigned frame = 0; frame < 16; ++frame) {
        const bool *e = &m->edges[102 + frame * 54];
        unsigned reg = frame == 15 ? 0x7c : regs[frame % 5];
        bool parity = false;
        for (unsigned i = 0; i < 7; ++i) {
            bool v = (reg >> (6 - i)) & 1U;
            assert(e[1 + i] == v);
            parity ^= v;
        }
        assert(!e[8]); // every frame is a READ, without exception
        assert(e[9] == parity);
        for (unsigned i = 10; i <= 12; ++i) assert(e[i]);
        assert(!e[13] && !e[14]);
        for (unsigned i = 15; i <= 50; ++i) assert(e[i]); // released all target bits
        assert(e[51] && !e[52] && !e[53]);
    }
}
static void save_trace(void *ctx, const rv_trace_t *t) {
    model_t *m = ctx;
    assert(m->traces < sizeof(m->trace) / sizeof(m->trace[0]));
    m->trace[m->traces++] = *t;
}
int main(void) {
    model_t m;
    rv_bus_t b = setup(&m, 0);
    rv_identity_t id;
    assert(rv_identify(&b, 3300, false, true, &id) == RV_BOARD_UNVERIFIED);
    assert(rv_identify(&b, 3300, true, false, &id) == RV_WIRING_UNCHECKED);
    assert(rv_identify(&b, 2999, true, true, &id) == RV_VOLTAGE);
    assert(rv_identify(&b, 3451, true, true, &id) == RV_VOLTAGE);
    assert(!m.enable_count && !m.edge_count);
    assert(rv_identify(&b, 3300, true, true, &id) == RV_OK);
    assert(id.chip_id == 0x30300504U && id.completed_samples == 3);
    assert(!b.active && !m.enabled && m.pos == m.count);
    verify_frames(&m);
    unsigned calls = m.calls;
    const rv_result_t expected[] = {RV_OK, RV_NO_RESPONSE, RV_NO_RESPONSE,
        RV_WRONG_FAMILY, RV_UNSTABLE, RV_PARITY, RV_DM_INACTIVE,
        RV_AUTH_REQUIRED, RV_ABSTRACT_BUSY, RV_STATUS_INVALID,
        RV_STATUS_INVALID, RV_STATUS_INVALID, RV_STATUS_INVALID, RV_UNSTABLE,
        RV_UNSTABLE, RV_UNSTABLE, RV_PARITY, RV_OK};
    for (unsigned mode = 1; mode < sizeof(expected) / sizeof(expected[0]); ++mode) {
        b = setup(&m, (int)mode);
        b.trace_sink = save_trace;
        b.trace_ctx = &m;
        assert(rv_identify(&b, 3300, true, true, &id) == expected[mode]);
        assert(!m.enabled && !b.active && m.enable_count == 1);
        if (mode == 6) {
            assert(!id.dmactive_set && id.dmcontrol == 0 && m.pos == m.count);
            verify_frames(&m); // inactive identification still emits READs only
            assert(m.traces == 16 && m.trace[0].sequence == 1);
        }
        if (mode == 16) {
            assert(m.traces == 2 && id.completed_samples == 0 && id.chip_id == 0x30300504U);
            const rv_trace_t *t = &m.trace[1];
            assert(t->sequence == 2 && t->reg == 0x10 && !t->write && t->word == UINT32_MAX);
            assert(t->parity_expected == 0 && t->parity_received == 1 && t->data_bits == 32);
            assert(!strcmp(t->stage, "data_parity_check") && t->result == RV_PARITY);
            assert(t->header_aux_bits == 3 && t->tail_aux_bits == 3);
            assert(t->header_aux == 2 && t->tail_aux == 2);
        }
        if (mode == 17) {
            assert(m.traces == 16 && id.completed_samples == 3);
            assert(id.cpbr_attempted && !id.cpbr_valid && id.cpbr_result == RV_PARITY);
            assert(id.cpbr == UINT32_MAX && !strcmp(id.cpbr_failure_stage, "data_parity_check"));
            for (unsigned i = 0; i < 15; ++i)
                assert(m.trace[i].result == RV_OK && !m.trace[i].optional && !m.trace[i].write);
            const rv_trace_t *t = &m.trace[15];
            assert(t->reg == 0x7c && t->optional && t->result == RV_PARITY);
            assert(t->header_aux == 7 && t->tail_aux == 7 && t->word == UINT32_MAX);
            assert(!strcmp(t->phase, "optional_cpbr"));
            verify_frames(&m); // exactly one optional attempt, no retry or later DMI
        }
    }
    for (unsigned call = 1; call <= calls; ++call) {
        b = setup(&m, 0);
        m.fail_at = call;
        assert(rv_identify(&b, 3300, true, true, &id) == RV_IO);
        assert(!m.enabled && !b.active && m.enable_count == 1);
    }
    b = setup(&m, 0);
    m.timeout = true;
    assert(rv_identify(&b, 3300, true, true, &id) == RV_TIMEOUT);
    assert(!m.enabled);
    b = setup(&m, 0);
    m.stuck_clock = true;
    assert(rv_identify(&b, 3300, true, true, &id) == RV_LINE_LOW);
    assert(!m.enabled);
    b = setup(&m, 0);
    m.queue[0] = false;
    assert(rv_identify(&b, 3300, true, true, &id) == RV_LINE_LOW);
    assert(!m.enabled);
    b = setup(&m, 0);
    b.config.sample_low = true;
    assert(rv_identify(&b, 3300, true, true, &id) == RV_OK);
    verify_frames(&m);
    b = setup(&m, 0);
    b.config.idle_clock_high = true;
    b.config.half_us = 250;
    b.config.turnaround_us = 50;
    assert(rv_identify(&b, RV_MIN_MV, true, true, &id) == RV_OK);
    assert(!m.enabled && m.pos == m.count);
    b = setup(&m, 0);
    assert(rv_identify(&b, RV_MAX_MV, true, true, &id) == RV_OK);
    b = setup(&m, 6);
    assert(rv_diagnose(&b, 3300, true, true, &id) == RV_OK);
    assert(id.dmcontrol == 0 && !id.dmactive_set && id.completed_samples == 3);
    verify_frames(&m);
    printf("PASS: five stable required reads, optional CPBR parity reported, inactive diagnosis, gates, stability, timeouts; "
           "%u GPIO failure positions released without retry\n", calls);
    return 0;
}
