/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000078ba; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000078ba; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_000078ba(void)

{
  gp = &__global_pointer;
  tim2_buzzer_pwm_config(0x17f,99,0xbf);
  return;
}

