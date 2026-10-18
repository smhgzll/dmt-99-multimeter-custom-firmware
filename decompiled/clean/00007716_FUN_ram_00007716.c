/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00007716; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007716; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007716(void)

{
  undefined *puStack_18;
  undefined1 uStack_16;
  undefined4 uStack_14;
  
  gp = &__global_pointer;
  puStack_18 = &mhpmevent13;
  uStack_16 = 3;
  uStack_14 = 1;
  NVIC_Init(&puStack_18);
  return;
}

