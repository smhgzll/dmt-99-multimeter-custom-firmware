/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00007918; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007918; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007918(void)

{
  gp = &__global_pointer;
  FUN_ram_000078ba();
  delay_ms(0xfa);
  buzzer_enable(0);
  return;
}

