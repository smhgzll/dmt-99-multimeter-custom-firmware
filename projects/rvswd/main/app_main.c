// SPDX-License-Identifier: MIT
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "soc/uart_pins.h"
#if !defined(CONFIG_IDF_TARGET_ESP32) || !defined(CONFIG_ESP_CONSOLE_UART_DEFAULT)
#error "This adapter requires classic ESP32 and the default UART0 console"
#endif
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rvswd_ro.h"
#include "rvswd_diag.h"
#include "rvswd_memory_lab.h"

#define CLK GPIO_NUM_25
#define DIO GPIO_NUM_26
_Static_assert(GPIO_IS_VALID_OUTPUT_GPIO(CLK), "SWCLK must support GPIO output");
_Static_assert(GPIO_IS_VALID_OUTPUT_GPIO(DIO), "SWDIO must support GPIO output");
_Static_assert(CLK != U0TXD_GPIO_NUM && CLK != U0RXD_GPIO_NUM &&
               DIO != U0TXD_GPIO_NUM && DIO != U0RXD_GPIO_NUM,
               "RVSWD must not overlap UART0");
static rv_bus_t bus;
static bool initialized;
// Bounded snapshots and polling, printed only after GPIO release.
static rv_trace_t traces[1024];
static unsigned trace_count;
static bool trace_overflow;
static void save_trace(void *ctx, const rv_trace_t *trace) {
    (void)ctx;
    if (trace_count < sizeof(traces) / sizeof(traces[0])) traces[trace_count++] = *trace;
    else trace_overflow = true;
    // Parked between frames: let the idle task run during longer snapshots.
    vTaskDelay(1);
}

static bool console_init(void) {
    // Classic ESP32 ROM/boot-console UART0: TX=GPIO1, RX=GPIO3.
    const uart_config_t cfg = {
        .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    if (uart_param_config(UART_NUM_0, &cfg) != ESP_OK ||
        uart_set_pin(UART_NUM_0, U0TXD_GPIO_NUM, U0RXD_GPIO_NUM,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK ||
        uart_driver_install(UART_NUM_0, 2048, 2048, 0, NULL, 0) != ESP_OK)
        return false;
    uart_vfs_dev_use_driver(UART_NUM_0);
    return true;
}

static int console_read_byte(uint8_t *byte) {
    return uart_read_bytes(UART_NUM_0, byte, 1, pdMS_TO_TICKS(20));
}

static bool release_pins(void *ctx) {
    (void)ctx;
    // Do not use gpio_reset_pin(): its default pull-up can feed an unpowered target.
    // Disable BOTH outputs before further configuration, attempting every cleanup.
    esp_err_t a = gpio_set_direction(CLK, GPIO_MODE_INPUT);
    esp_err_t b = gpio_set_direction(DIO, GPIO_MODE_INPUT);
    esp_err_t c = gpio_set_pull_mode(CLK, GPIO_FLOATING);
    esp_err_t d = gpio_set_pull_mode(DIO, GPIO_FLOATING);
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << CLK) | (1ULL << DIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t e = gpio_config(&cfg); // select the GPIO mux as well
    return a == ESP_OK && b == ESP_OK && c == ESP_OK && d == ESP_OK && e == ESP_OK;
}
static bool enable_pins(void *ctx) {
    (void)ctx;
    // Preload released latches BEFORE enabling the output transistors.
    if (gpio_set_level(CLK, 1) != ESP_OK || gpio_set_level(DIO, 1) != ESP_OK)
        return false;
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << CLK) | (1ULL << DIO),
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&cfg) == ESP_OK;
}
static bool set_pin(void *ctx, unsigned line, bool high) {
    (void)ctx;
    return gpio_set_level(line == RV_SWCLK ? CLK : DIO, high) == ESP_OK;
}
static bool get_pin(void *ctx, unsigned line, bool *level) {
    (void)ctx;
    *level = gpio_get_level(line == RV_SWCLK ? CLK : DIO) != 0;
    return true;
}
static void delay_us(void *ctx, uint32_t us) {
    (void)ctx;
    esp_rom_delay_us(us);
}
static uint64_t now_us(void *ctx) {
    (void)ctx;
    return (uint64_t)esp_timer_get_time();
}
static bool board_verified(void) {
#ifdef CONFIG_DIAG_BOARD_PINS_VERIFIED
    return strcmp(CONFIG_DIAG_BOARD_NAME, "UNVERIFIED") && strlen(CONFIG_DIAG_BOARD_NAME);
#else
    return false;
#endif
}
static void json_string(const char *s) {
    putchar('"');
    for (; *s; ++s) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') putchar('\\');
        if (c < 0x20) printf("\\u%04x", c);
        else putchar(c);
    }
    putchar('"');
}
static bool dm_activation_enabled(void) {
#ifdef CONFIG_DIAG_ALLOW_DM_ACTIVATION
    return true;
#else
    return false;
#endif
}
static bool halt_resume_enabled(void) {
#ifdef CONFIG_DIAG_ALLOW_HALT_RESUME
    return true;
#else
    return false;
#endif
}
static bool memory_lab_enabled(void) {
#ifdef RV_ABSTRACT_MEMORY_LAB
    return true;
#else
    return false;
#endif
}
static bool progbuf_lab_enabled(void) {
#ifdef RV_PROGBUF_LAB
    return true;
#else
    return false;
#endif
}
static void info(void) {
    printf("{\"event\":\"info\",\"protocol\":3,\"build\":\"read-only-diagnostic\","
           "\"idf\":\"5.5.5\",\"adapter_target\":\"esp32\",\"target_validation\":\"unvalidated\","
           "\"physical_validation_completed\":false,\"board_gate_enabled\":%s,"
           "\"board_verified\":%s,\"board\":",
           board_verified() ? "true" : "false", board_verified() ? "true" : "false");
    json_string(CONFIG_DIAG_BOARD_NAME);
    printf(",\"console\":\"uart0\",\"baud\":%d,\"console_tx\":%d,\"console_rx\":%d",
           CONFIG_ESP_CONSOLE_UART_BAUDRATE, U0TXD_GPIO_NUM, U0RXD_GPIO_NUM);
    printf(",\"initialized\":%s,\"gpio_clk\":%d,\"gpio_dio\":%d,"
           "\"half_us\":%" PRIu32 ",\"turnaround_us\":%" PRIu32 ","
           "\"samples\":%u,\"sample_low\":%s,\"idle_clock_high\":%s,"
           "\"memory_read\":true,\"dmi_write\":true,\"target_memory_write\":false,"
           "\"flash_write\":false,\"flash_dump\":false,\"reset_command\":false,\"vref_adc\":false,\"diagnostic_modes\":true,"
           "\"identify_writes\":false,\"dmactive_requires_explicit_command\":true,"
           "\"mandatory_registers\":5,\"cpbr_optional\":true,"
           "\"dmi_trace\":\"buffered_after_release\",\"dm_activation_enabled\":%s,"
           "\"halt_resume_enabled\":%s,\"abstract_memory_lab\":%s,"
           "\"abstract_memory_running_validated\":false,\"memory_lab_requires_halted\":true,"
           "\"progbuf_lab\":%s,\"halt_memory_cycle\":%s,\"running_spare_lab\":%s,\"main_stack_free_min_bytes\":%u}\n",
           initialized ? "true" : "false", CLK, DIO, bus.config.half_us,
           bus.config.turnaround_us, bus.config.samples,
           bus.config.sample_low ? "true" : "false",
           bus.config.idle_clock_high ? "true" : "false",
           dm_activation_enabled() ? "true" : "false",
           halt_resume_enabled() ? "true" : "false",
           memory_lab_enabled() ? "true" : "false",
           progbuf_lab_enabled() ? "true" : "false",
           memory_lab_enabled() && halt_resume_enabled() ? "true" : "false",
           memory_lab_enabled() ? "true" : "false", (unsigned)uxTaskGetStackHighWaterMark(NULL));
}
static bool parse_unsigned(const char *s, unsigned *out) {
    if (!s || !*s || strlen(s) > 5) return false;
    for (const char *p = s; *p; ++p) if (!isdigit((unsigned char)*p)) return false;
    *out = (unsigned)strtoul(s, NULL, 10);
    return true;
}
static bool valid_request(const char *s) {
    if (!s || strlen(s) != 16) return false;
    for (const char *p = s; *p; ++p) if (!isxdigit((unsigned char)*p)) return false;
    return true;
}
static void print_traces(const char *request) {
    for (unsigned i = 0; i < trace_count; ++i) {
        const rv_trace_t *t = &traces[i];
        const char *parity = t->write ? (t->data_parity_sent ? "tx_only" : "not_completed") :
            t->parity_received < 0 ? "not_received" :
            t->parity_received == t->parity_expected ? "ok" : "mismatch";
        printf("{\"event\":\"dmi\",\"request\":\"%s\",\"sequence\":%" PRIu32 ","
               "\"phase\":\"%s\",\"sample\":%" PRIu32 ",\"address\":\"%02x\","
               "\"operation\":\"%s\",\"word\":\"%08" PRIx32 "\",\"data_bits\":%u,"
               "\"optional\":%s,"
               "\"ack\":\"uninterpreted_short_frame\",\"aux_header\":%u,"
               "\"aux_header_bits\":%u,\"aux_tail\":%u,\"aux_tail_bits\":%u,"
               "\"header_parity_tx\":%u,\"header_parity_sent\":%s,"
               "\"parity\":\"%s\",\"parity_expected\":%d,"
               "\"parity_received\":%d,\"stage\":\"%s\",\"result\":\"%s\"}\n",
               request, t->sequence, t->phase, t->sample, t->reg,
               t->write ? "write" : "read", t->word, t->data_bits,
               t->optional ? "true" : "false",
               t->header_aux, t->header_aux_bits, t->tail_aux, t->tail_aux_bits,
               t->header_parity, t->header_parity_sent ? "true" : "false",
               parity, t->parity_expected, t->parity_received,
               t->stage, rv_result_name(t->result));
    }
}
static void diagnostic(const char *cmd, const char *request, unsigned mv,
                       bool wiring, unsigned select) {
    // Single console task: large reports must not consume its printf stack.
    static rv_diag_report_t report;
    const rv_diag_options_t opts = {.select = select,
        .halt_cycle = !strcmp(cmd, "halt-cycle"), .halt_resume_enabled = halt_resume_enabled()};
    trace_count = 0;
    trace_overflow = false;
    bus.trace_sink = save_trace;
    rv_result_t r = rv_capabilities(&bus, mv, board_verified(), wiring, &opts, &report);
    bus.trace_sink = NULL;
    print_traces(request);
    printf("{\"event\":\"%s\",\"request\":\"%s\",\"result\":\"%s\","
           "\"operation_result\":\"%s\",\"cleanup_result\":\"%s\","
           "\"failure_class\":\"%s\",\"select\":%u,"
           "\"chip_id\":\"%08" PRIx32 "\",\"dmcontrol\":\"%08" PRIx32 "\","
           "\"dmstatus\":\"%08" PRIx32 "\",\"hartinfo\":\"%08" PRIx32 "\","
           "\"abstractcs\":\"%08" PRIx32 "\",\"completed_samples\":%u,"
           "\"abstractauto\":\"%08" PRIx32 "\",\"abstractauto_valid\":%s,"
           "\"sbcs\":\"%08" PRIx32 "\",\"sbcs_valid\":%s,"
           "\"abstract_cmderr\":%u,\"sberror\":%u,\"read_protection\":\"unknown\","
           "\"capabilities\":{",
           cmd, request, rv_result_name(r), rv_result_name(report.operation_result),
           rv_result_name(report.cleanup_result), rv_failure_class(report.operation_result), select,
           report.identity.chip_id, report.identity.dmcontrol, report.identity.dmstatus,
           report.identity.hartinfo, report.identity.abstractcs, report.identity.completed_samples,
           report.abstractauto, report.abstractauto_valid ? "true" : "false",
           report.sbcs, report.sbcs_valid ? "true" : "false", report.abstract_cmderr, report.sberror);
    for (unsigned i = 0; i < RV_CAP_COUNT; ++i) {
        const rv_capability_t *c = &report.capabilities[i];
        printf("%s\"%s\":{\"status\":\"%s\",\"reason\":\"%s\",\"failure_class\":\"%s\"}",
               i ? "," : "", rv_cap_name((rv_cap_id_t)i), rv_cap_state_name(c->state),
               rv_result_name(c->reason), rv_failure_class(c->reason));
    }
    printf("},\"pc_valid\":%s,\"pc\":\"%08" PRIx32 "\",\"gpr_valid_mask\":\"%08" PRIx32 "\",\"gpr\":[",
           report.pc_valid ? "true" : "false", report.pc, report.gpr_valid_mask);
    for (unsigned i = 0; i < 32; ++i) printf("%s\"%08" PRIx32 "\"", i ? "," : "", report.gpr[i]);
    printf("],\"snapshots\":[");
    for (unsigned i = 0; i < report.word_count; ++i) {
        const rv_snapshot_word_t *w = &report.words[i];
        printf("%s{\"name\":\"%s\",\"address\":\"%08" PRIx32 "\",\"value\":\"%08" PRIx32 "\",\"width_bits\":%u}",
               i ? "," : "", w->name, w->address, w->value, w->width_bits);
    }
    printf("],\"halt_requested\":%s,\"halt_observed\":%s,\"resume_requested\":%s,"
           "\"resume_observed\":%s,\"control_restored\":%s,\"data0_restored\":%s,"
           "\"sba_config_restored\":%s,\"target_state_uncertain\":%s,"
           "\"final_status_valid\":%s,\"final_dmstatus\":\"%08" PRIx32 "\","
           "\"final_control_valid\":%s,\"final_dmcontrol\":\"%08" PRIx32 "\","
           "\"pins_released\":%s,\"trace_count\":%u,\"trace_overflow\":%s}\n",
           report.halt_requested ? "true" : "false", report.halt_observed ? "true" : "false",
           report.resume_requested ? "true" : "false", report.resume_observed ? "true" : "false",
           report.control_restored ? "true" : "false", report.data0_restored ? "true" : "false",
           report.sba_config_restored ? "true" : "false", report.target_state_uncertain ? "true" : "false",
           report.final_status_valid ? "true" : "false", report.final_dmstatus,
           report.final_control_valid ? "true" : "false", report.final_dmcontrol,
           report.pins_released ? "true" : "false", trace_count, trace_overflow ? "true" : "false");
}
static void memory_lab(const char *request, unsigned mv, bool wiring, bool running_spare, bool cycle, bool progbuf, unsigned profile) {
    static rv_memory_lab_report_t o;
    trace_count = 0;
    trace_overflow = false;
    bus.trace_sink = save_trace;
    static rv_diag_report_t cycle_report;
    memset(&cycle_report, 0, sizeof(cycle_report));
    rv_result_t r;
    if (cycle) {
        rv_diag_options_t opts = {.halt_cycle = true, .halt_resume_enabled = halt_resume_enabled(),
                                 .memory_lab_cycle = true, .progbuf_lab_cycle = progbuf, .memory_profile = profile};
        r = rv_capabilities(&bus, mv, board_verified(), wiring, &opts, &cycle_report);
        o = cycle_report.memory_lab;
        o.identity = cycle_report.identity;
        o.operation_result = cycle_report.operation_result;
        o.cleanup_result = cycle_report.cleanup_result;
        o.final_status = cycle_report.final_dmstatus;
        o.final_control = cycle_report.final_dmcontrol;
        o.final_valid = cycle_report.final_status_valid && cycle_report.final_control_valid;
        o.state_uncertain = cycle_report.target_state_uncertain || o.state_uncertain;
        o.pins_released = cycle_report.pins_released;
    } else r = running_spare
        ? rv_memory_lab_running_spare(&bus, mv, board_verified(), wiring, true, &o)
        : rv_memory_lab(&bus, mv, board_verified(), wiring, true, &o);
    bus.trace_sink = NULL;
    print_traces(request);
    printf("{\"event\":\"lab-memory\",\"request\":\"%s\",\"result\":\"%s\","
           "\"operation_result\":\"%s\",\"cleanup_result\":\"%s\",\"failure_class\":\"%s\","
           "\"chip_id\":\"%08" PRIx32 "\",\"cmderr\":%u,\"command_attempts\":%u,"
           "\"data_restored\":%s,\"final_valid\":%s,\"final_dmstatus\":\"%08" PRIx32 "\","
           "\"final_dmcontrol\":\"%08" PRIx32 "\",\"target_state_uncertain\":%s,"
           "\"pins_released\":%s,\"trace_count\":%u,\"trace_overflow\":%s,"
           "\"recovery_verified\":false,\"passes\":[",
           request, rv_result_name(r), rv_result_name(o.operation_result),
           rv_result_name(o.cleanup_result), rv_failure_class(o.operation_result),
           o.identity.chip_id, o.cmderr, o.command_attempts,
           o.data_restored ? "true" : "false", o.final_valid ? "true" : "false",
           o.final_status, o.final_control, o.state_uncertain ? "true" : "false",
           o.pins_released ? "true" : "false", trace_count, trace_overflow ? "true" : "false");
    for (unsigned p = 0; p < RV_LAB_PASSES; ++p) {
        printf("%s[", p ? "," : "");
        for (unsigned i = 0; i < o.completed[p]; ++i)
            printf("%s{\"address\":\"%08" PRIx32 "\",\"value\":\"%08" PRIx32 "\"}",
                   i ? "," : "", rv_lab_address(o.profile, i), o.words[p][i]);
        putchar(']');
    }
    printf("],\"halt_cycle\":%s,\"halt_observed\":%s,\"resume_observed\":%s,"
           "\"profile\":%u,\"resume_ack_observed\":%s,\"gprs_restored\":%s,\"program_restored\":%s,\"pc_unchanged\":%s,\"progbuf\":%s,"
           "\"control_restored\":%s,\"initial_cmderr\":%u,\"cmderr_clear_attempted\":%s,\"cmderr_cleared\":%s}\n",
           cycle ? "true" : "false", cycle_report.halt_observed ? "true" : "false",
           cycle_report.resume_observed ? "true" : "false", profile, cycle_report.resume_ack_observed ? "true" : "false",
           o.gprs_restored ? "true" : "false", o.program_restored ? "true" : "false",
           o.pc_unchanged ? "true" : "false", progbuf ? "true" : "false",
           cycle_report.control_restored ? "true" : "false",
           cycle_report.initial_cmderr, cycle_report.cmderr_clear_attempted ? "true" : "false",
           cycle_report.cmderr_cleared ? "true" : "false");
}
static void handle(char *line) {
    char *save = NULL;
    char *cmd = strtok_r(line, " \t", &save);
    if (!cmd) return;
    if (!strcmp(cmd, "hello") || !strcmp(cmd, "info")) {
        info();
    } else if (!strcmp(cmd, "lab-memory") || !strcmp(cmd, "lab-memory-running-spare") || !strcmp(cmd, "lab-memory-halt-cycle") || !strcmp(cmd, "lab-progbuf-halt-cycle") || !strcmp(cmd, "lab-progbuf-word")) {
        bool running_spare = !strcmp(cmd, "lab-memory-running-spare");
        bool word = !strcmp(cmd, "lab-progbuf-word");
        bool progbuf = word || !strcmp(cmd, "lab-progbuf-halt-cycle");
        bool cycle = progbuf || !strcmp(cmd, "lab-memory-halt-cycle");
        char *request = strtok_r(NULL, " \t", &save);
        char *mv_text = strtok_r(NULL, " \t", &save);
        char *wiring = strtok_r(NULL, " \t", &save);
        char *token = strtok_r(NULL, " \t", &save);
        unsigned mv, profile = 0;
        bool profile_ok = !word || (parse_unsigned(strtok_r(NULL, " \t", &save), &profile) && profile >= 1 && profile <= RV_LAB_PROFILE_MAX);
        if (!profile_ok || !initialized || !valid_request(request) || !parse_unsigned(mv_text, &mv) ||
            !wiring || strcmp(wiring, "PINS25_26_WIRING_CHECKED") || !token ||
            strcmp(token, progbuf ? "EXPLICIT_PROGBUF_LOAD_RESUME" : cycle ? "EXPLICIT_HALT_MEMORY_RESUME" : running_spare ? "EXPLICIT_RUNNING_SPARE_MEMORY_LAB" : "EXPLICIT_HALTED_MEMORY_LAB") ||
            strtok_r(NULL, " \t", &save)) {
            rv_release(&bus);
            puts("{\"event\":\"error\",\"result\":\"invalid_command\"}");
        } else if (!memory_lab_enabled() || (cycle && !halt_resume_enabled()) || (progbuf && !progbuf_lab_enabled())) {
            rv_release(&bus);
            puts("{\"event\":\"error\",\"result\":\"disabled_by_policy\"}");
        } else memory_lab(request, mv, true, running_spare, cycle, progbuf, profile);
    } else if (!strcmp(cmd, "stop")) {
        rv_result_t r = rv_release(&bus);
        printf("{\"event\":\"stopped\",\"result\":\"%s\"}\n", rv_result_name(r));
    } else if (!strcmp(cmd, "identify") || !strcmp(cmd, "diagnose") ||
               !strcmp(cmd, "activate-dm") || !strcmp(cmd, "capabilities") || !strcmp(cmd, "halt-cycle")) {
        // Exactly one fresh operator attestation per bounded probe; never saved in NVS.
        char *request = strtok_r(NULL, " \t", &save);
        char *mv_string = strtok_r(NULL, " \t", &save);
        char *attestation = strtok_r(NULL, " \t", &save);
        char *option = strtok_r(NULL, " \t", &save);
        char *mask_text = NULL;
        bool new_diag = !strcmp(cmd, "capabilities") || !strcmp(cmd, "halt-cycle");
        bool halt_cycle = !strcmp(cmd, "halt-cycle");
        bool activate = !strcmp(cmd, "activate-dm");
        bool changes_state = halt_cycle || activate;
        bool explicit_change = !changes_state || (option &&
            ((!halt_cycle && !strcmp(option, "EXPLICIT_DM_ACTIVATION")) ||
             (!activate && !strcmp(option, "EXPLICIT_HALT_RESUME"))));
        if (new_diag) mask_text = changes_state ? strtok_r(NULL, " \t", &save) : option;
        char *extra = strtok_r(NULL, " \t", &save);
        unsigned select = 0;
        unsigned mv = 0;
        bool wiring = attestation && !strcmp(attestation, "PINS25_26_WIRING_CHECKED");
        if (!initialized || !valid_request(request) || !parse_unsigned(mv_string, &mv) || extra ||
            (!new_diag && !changes_state && option) || !explicit_change ||
            (new_diag && (!parse_unsigned(mask_text, &select) || select > RV_DIAG_MASK))) {
            rv_release(&bus);
            printf("{\"event\":\"error\",\"result\":\"invalid_command\"}\n");
            return;
        }
        if ((activate && !dm_activation_enabled()) || (halt_cycle && !halt_resume_enabled())) {
            rv_release(&bus);
            printf("{\"event\":\"error\",\"result\":\"disabled_by_policy\"}\n");
            return;
        }
        if (new_diag) {
            diagnostic(cmd, request, mv, wiring, select);
            return;
        }
        rv_identity_t id;
        trace_count = 0;
        trace_overflow = false;
        bus.trace_sink = save_trace;
        rv_result_t r;
        if (!strcmp(cmd, "diagnose")) r = rv_diagnose(&bus, mv, board_verified(), wiring, &id);
        else if (!strcmp(cmd, "activate-dm")) r = rv_activate_dm(&bus, mv, board_verified(), wiring, &id);
        else r = rv_identify(&bus, mv, board_verified(), wiring, &id);
        bus.trace_sink = NULL;
        print_traces(request); // all GPIOs are released before UART logging
        const char *failure_stage = "none";
        if (r != RV_OK) {
            failure_stage = !strcmp(cmd, "activate-dm") && id.activation_cleanup_result != RV_OK ? "gpio_release" :
                !trace_count ? "initialization_or_sync" :
                traces[trace_count - 1].result == r ? traces[trace_count - 1].stage :
                r == RV_IO ? "gpio_release" : "register_validation";
        }
        printf("{\"event\":\"%s\",\"request\":\"%s\",\"result\":\"%s\","
               "\"failure_class\":\"%s\","
               "\"operation_result\":\"%s\",\"cleanup_result\":\"%s\","
               "\"measured_mv_operator\":%u,\"completed_samples\":%u,"
               "\"chip_id\":\"%08" PRIx32 "\",\"dmcontrol\":\"%08" PRIx32 "\","
               "\"dmstatus\":\"%08" PRIx32 "\",\"hartinfo\":\"%08" PRIx32 "\","
               "\"abstractcs\":\"%08" PRIx32 "\",\"cpbr\":\"%08" PRIx32 "\","
               "\"mandatory_registers\":5,\"cpbr_optional\":true,\"cpbr_status\":\"%s\","
               "\"cpbr_result\":\"%s\",\"cpbr_failure_stage\":\"%s\","
               "\"last_register\":%u,\"last_word\":\"%08" PRIx32 "\","
               "\"last_word_valid\":%s,\"dmactive_set\":%s,\"family_hint\":\"%s\","
               "\"dmactive_write_attempted\":%s,\"dmactive_already_active\":%s,"
               "\"dmcontrol_readback_valid\":%s,\"dmstatus_readback_valid\":%s,"
               "\"activation_state_uncertain\":%s,"
               "\"exact_part_verified\":false,\"read_protection\":\"unknown\","
               "\"memory_read\":false,\"trace_count\":%u,\"trace_overflow\":%s,"
               "\"failure_stage\":\"%s\",\"pins_released\":%s}\n",
               cmd, request, rv_result_name(r), rv_failure_class(r),
               rv_result_name(id.activation_operation_result),
               rv_result_name(id.activation_cleanup_result), mv,
               id.completed_samples, id.chip_id, id.dmcontrol,
               id.dmstatus, id.hartinfo, id.abstractcs, id.cpbr,
               !id.cpbr_attempted ? "not_tested" : id.cpbr_valid ? "available" : "unavailable",
               id.cpbr_attempted ? rv_result_name(id.cpbr_result) : "not_tested",
               id.cpbr_failure_stage ? id.cpbr_failure_stage : "none", id.last_register,
               id.last_word, id.last_word_valid ? "true" : "false",
               id.dmactive_set ? "true" : "false",
               r == RV_OK ? "CH32V303 candidate" : "unconfirmed",
               id.dmactive_write_attempted ? "true" : "false",
               id.dmactive_already_active ? "true" : "false",
               id.dmcontrol_readback_valid ? "true" : "false",
               id.dmstatus_readback_valid ? "true" : "false",
               id.activation_state_uncertain ? "true" : "false",
               trace_count, trace_overflow ? "true" : "false",
               failure_stage,
               id.pins_released ? "true" : "false");
    } else {
        rv_release(&bus);
        printf("{\"event\":\"error\",\"result\":\"unsupported_command\","
               "\"commands\":[\"hello\",\"diagnose\",\"identify\",\"capabilities\",\"halt-cycle\",\"activate-dm\",\"stop\"]}\n");
    }
}

void app_main(void) {
    // First application action, before console setup or logging.
    bool released = release_pins(NULL);
    const rv_io_t io = {.enable = enable_pins, .set = set_pin, .get = get_pin,
        .release = release_pins, .delay_us = delay_us, .now_us = now_us};
    const rv_config_t config = {
        .half_us = CONFIG_DIAG_HALF_PERIOD_US,
        .turnaround_us = CONFIG_DIAG_TURNAROUND_US,
        .samples = CONFIG_DIAG_IDENTIFY_SAMPLES,
#ifdef CONFIG_DIAG_SAMPLE_LOW
        .sample_low = true,
#endif
#ifdef CONFIG_DIAG_IDLE_CLOCK_HIGH
        .idle_clock_high = true,
#endif
    };
    initialized = rv_init(&bus, &io, &config) == RV_OK && released;
    if (!console_init()) {
        release_pins(NULL);
        return;
    }
    setvbuf(stdout, NULL, _IONBF, 0);
    info();
    char line[128];
    size_t used = 0;
    bool discard = false;
    while (true) {
        uint8_t c;
        int n = console_read_byte(&c);
        if (n <= 0) continue;
        if (c == '\r' || c == '\n') {
            if (discard) {
                rv_release(&bus);
                printf("{\"event\":\"error\",\"result\":\"line_too_long\"}\n");
            } else {
                line[used] = '\0';
                handle(line);
            }
            used = 0;
            discard = false;
        } else if (c < 0x20 || c > 0x7e) {
            discard = true; // never execute a prefix of a malformed command
        } else if (used < sizeof(line) - 1 && !discard) {
            line[used++] = (char)c;
        } else {
            discard = true;
        }
    }
}
