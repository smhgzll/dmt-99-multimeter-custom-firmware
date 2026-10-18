/* Original address: 0x0000a782; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000a782(void)

{
  uchar auStack_18 [8];
  
  gp = &__global_pointer;
  measurement_decode_segments(auStack_18);
  FUN_ram_0001014e(auStack_18[0] != '\x0e');
  return;
}

