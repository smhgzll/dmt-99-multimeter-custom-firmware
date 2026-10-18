/* Semantic review: high. Saves settings dims backlight drives PC1 low then infinite loop.
 * Address 0x0000c320; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c320; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Saves settings dims backlight drives PC1 low then infinite loop */

void power_down(void)

{
  FUN_ram_0000792e();
  display_fill(0);
  tim8_backlight_pwm_config(100,0x12bf,100);
  settings_commit_internal_flash();
  GPIO_ResetBits(&GPIOC,2);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

