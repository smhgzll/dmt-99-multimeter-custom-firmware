/* Original address: 0x000090e6; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_000090e6(uint param_1)

{
  int iVar1;
  
  gp = &__global_pointer;
  iVar1 = 1;
  for (; 9 < param_1; param_1 = param_1 / 10) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}

