#ifndef DMT_FIRMWARE_H
#define DMT_FIRMWARE_H
/* Target-only services of the standalone DMT99 firmware. Nothing here is
 * compiled into host tests or the simulator; host code reaches the same
 * application through the callback structures in application.h. */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Build options. Defaults are the release-candidate policy documented in
 * docs/FIRMWARE_RELEASE.md. */
#ifndef DMT_FW_SPI_FLASH_WRITES
#define DMT_FW_SPI_FLASH_WRITES 1   /* 0: never erase/program the W25Q64JV */
#endif
#ifndef DMT_FW_DIAGNOSTIC_ENTRY
#define DMT_FW_DIAGNOSTIC_ENTRY 0   /* 1: allow PC10+PC11 boot entry, sends A0 */
#endif
#ifndef DMT_FW_SETTINGS_WRITES
#define DMT_FW_SETTINGS_WRITES 1    /* 0: never erase/program internal 0x08040000 */
#endif
#ifndef DMT_FW_BRINGUP
#define DMT_FW_BRINGUP 0            /* 1: log keys, UART frames/commands and status on PA2 */
#endif

enum {FW_CLOCK_HSE_PLL=1,FW_CLOCK_HSI_PLL=2};
extern volatile uint8_t fw_clock_source;

/* delay.c: 1a626/1a64c. SysTick runs free at HCLK/8; delays are ISR-safe. */
void fw_delay_init(void);
void fw_delay_ms(uint32_t milliseconds);
void fw_delay_us(uint32_t microseconds);
uint64_t fw_ticks(void);
uint32_t fw_millis(void);

/* debug.c: USART2 TX on PA2, 115200 8N1 (1a682, 1a6e2, printf 1aede). */
void fw_debug_init(uint32_t baud);
void fw_debug_write(const char *,size_t);
int fw_vsnprintf(char *,size_t,const char *,va_list);
int fw_printf(const char *,...) __attribute__((format(printf,1,2)));

/* battery.c: 770a, 767a, 7562, 7598, 75f0. */
void fw_battery_inputs_init(void);
uint16_t fw_adc_read_averaged(uint8_t channel,uint8_t samples);
int fw_battery_percent(void);
bool fw_charging(void);

/* fault.c */
typedef struct {uint32_t magic,mcause,mepc,mtval,sp,count,reset_flags;} fw_fault_record;
void fw_fault_boot_report(void);
uint32_t fw_stack_unused(void);
void fw_system_reset(void) __attribute__((noreturn));

/* version.c */
extern const char fw_version_text[];
#endif
