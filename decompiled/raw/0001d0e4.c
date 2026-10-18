/* Original address: 0x0001d0e4; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001d0e4(uint param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  if (param_1 != 0) {
    iVar1 = FUN_ram_0001e4f8();
    if (iVar1 < 0xb) {
      uVar3 = param_1 >> (0xbU - iVar1 & 0x1f);
      iVar2 = param_1 << (iVar1 + 0x15U & 0x1f);
    }
    else {
      uVar3 = param_1 << (iVar1 - 0xbU & 0x1f);
      iVar2 = 0;
    }
    return CONCAT44(uVar3 & 0xfffff | (0x41eU - iVar1 & 0x7ff) << 0x14,iVar2);
  }
  return 0;
}

