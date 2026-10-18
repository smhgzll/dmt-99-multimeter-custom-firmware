/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00007f00; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007f00; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007f00(undefined4 param_1,undefined4 param_2,ushort param_3)

{
  gp = &__global_pointer;
  FUN_ram_00007e38();
  display_begin_pixels();
  display_write_data(param_3);
  return;
}

