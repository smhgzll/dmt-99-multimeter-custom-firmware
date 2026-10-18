/* Original address: 0x0001d072; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001d072(uint param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  if (param_1 == 0) {
    param_1 = 0;
    uVar4 = 0;
    uVar1 = 0;
    iVar2 = 0;
  }
  else {
    uVar1 = ((int)param_1 >> 0x1f ^ param_1) - ((int)param_1 >> 0x1f);
    iVar3 = FUN_ram_0001e4f8(uVar1);
    param_1 = param_1 >> 0x1f;
    uVar4 = 0x41eU - iVar3 & 0x7ff;
    if (iVar3 < 0xb) {
      iVar2 = uVar1 << (iVar3 + 0x15U & 0x1f);
      uVar1 = uVar1 >> (0xbU - iVar3 & 0x1f) & 0xfffff;
    }
    else {
      uVar1 = uVar1 << (iVar3 - 0xbU & 0x1f) & 0xfffff;
      iVar2 = 0;
    }
  }
  return CONCAT44(uVar1 | uVar4 << 0x14 | param_1 << 0x1f,iVar2);
}

