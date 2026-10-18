/* Original address: 0x0001e1c6; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001e1c6(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = param_1 >> 0x17 & 0xff;
  uVar1 = 0;
  if (uVar2 < 0x7f) {
    return uVar1;
  }
  if ((-1 < (int)param_1) && (uVar1 = 0xffffffff, uVar2 < 0x9f)) {
    uVar1 = param_1 & 0x7fffff | 0x800000;
    if (0x95 < uVar2) {
      return uVar1 << (uVar2 - 0x96 & 0x1f);
    }
    return uVar1 >> (0x96 - uVar2 & 0x1f);
  }
  return uVar1;
}

