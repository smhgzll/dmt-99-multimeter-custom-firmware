/* Semantic review: medium. Calls ADC and GPIO initialization routines.
 * Address 0x0000770a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000770a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: Calls ADC and GPIO initialization routines */

void battery_inputs_init(void)

{
  gp = &__global_pointer;
  adc_battery_init();
  charge_detect_gpio_init();
  return;
}

