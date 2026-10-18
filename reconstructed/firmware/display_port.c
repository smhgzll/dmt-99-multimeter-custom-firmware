/* Panel on the 8080-style 16-bit bus: PB0..15 data, PC3 reset, PC4 CS,
 * PC5 D/C, PC13 WR (0x7d6a/0x7dba). The controller commands in 0x87aa are an
 * ST7789-family sequence (porch B2, gate B7, VCOM BB, power D0, gamma E0/E1).
 * Each word is D/C, CS low, OUTDR, WR low, WR high, CS high, through separate
 * calls as in the original, so WR low/high phases stay well above the 15 ns
 * write-pulse minimum of that controller family at 144 MHz. */
#include "ch32v30x.h"
#include "firmware.h"
#include "display_port.h"
#include "wch_runtime.h"
dmt_display fw_display={.bus={0,recovered_display_set_pc,recovered_display_write_pb}};
static void delay(void *context,uint32_t ms){(void)context;fw_delay_ms(ms);}
void fw_display_init(void)
{
    recovered_display_gpio_init(delay,0);
    display_panel_sequence(&fw_display,delay,0);
}
