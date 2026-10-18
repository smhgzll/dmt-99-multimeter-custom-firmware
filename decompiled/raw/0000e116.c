/* Original address: 0x0000e116; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0000e116(char *param_1,int param_2,undefined4 param_3)

{
  gp = &__global_pointer;
  *(char *)(param_2 + 0x1b) = (char)((uint)param_3 >> 8);
  *(char *)(param_2 + 0x1a) = (char)param_3;
  if (*param_1 == '\x03') {
    *(char *)(param_2 + 0x14) = (char)((uint)param_3 >> 0x10);
    *(char *)(param_2 + 0x15) = (char)((uint)param_3 >> 0x18);
  }
  return;
}

