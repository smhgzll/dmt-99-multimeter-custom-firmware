/* Original address: 0x0000a7a4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000a7a4(void)

{
  gp = &__global_pointer;
  if (measurement_mode == '\t') {
    FUN_ram_0000a74e();
  }
  else if (measurement_mode == '\x13') {
    FUN_ram_0000a782();
  }
  else {
    FUN_ram_00009caa();
  }
  return;
}

