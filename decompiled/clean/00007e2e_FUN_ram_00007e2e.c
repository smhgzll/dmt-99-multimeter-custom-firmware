/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00007e2e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007e2e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007e2e(ushort param_1)

{
  gp = &__global_pointer;
  display_write_data(param_1);
  return;
}

