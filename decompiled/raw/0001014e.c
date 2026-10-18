/* Original address: 0x0001014e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0001014e(int param_1)

{
  char *pcVar1;
  
  gp = &__global_pointer;
  if (param_1 == 0) {
    if (DAT_ram_200001da == '\x17') {
      gp = &__global_pointer;
      return;
    }
    DAT_ram_200001da = 0x17;
    pcVar1 = "live.jpg";
  }
  else {
    if (param_1 != 1) {
      gp = &__global_pointer;
      return;
    }
    if (DAT_ram_200001da == '\x18') {
      gp = &__global_pointer;
      return;
    }
    DAT_ram_200001da = 0x18;
    pcVar1 = "live1.jpg";
  }
  resource_draw_jpeg(pcVar1,0,0);
  return;
}

