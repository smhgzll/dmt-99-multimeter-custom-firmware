/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000792e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000792e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000792e(void)

{
  gp = &__global_pointer;
  FUN_ram_000078ba();
  delay_ms(500);
  buzzer_enable(0);
  return;
}

