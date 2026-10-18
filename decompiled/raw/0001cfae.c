/* Original address: 0x0001cfae; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001cfae(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = param_2 >> 0x14 & 0x7ff;
  if (uVar2 < 0x3ff) {
    uVar2 = 0;
  }
  else {
    if (0x41d < uVar2) {
      return -((int)param_2 >> 0x1f) + 0x7fffffff;
    }
    uVar1 = param_2 & 0xfffff | 0x100000;
    if ((int)(0x433 - uVar2) < 0x20) {
      uVar1 = uVar1 << (uVar2 - 0x413 & 0x1f) | param_1 >> (0x433 - uVar2 & 0x1f);
    }
    else {
      uVar1 = uVar1 >> (0x413 - uVar2 & 0x1f);
    }
    uVar2 = -uVar1;
    if (-((int)param_2 >> 0x1f) == 0) {
      return uVar1;
    }
  }
  return uVar2;
}

