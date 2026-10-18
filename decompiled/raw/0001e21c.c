/* Original address: 0x0001e21c; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001e21c(uint param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  if (param_1 == 0) {
    param_1 = 0;
    uVar4 = 0;
    uVar1 = 0;
  }
  else {
    uVar1 = ((int)param_1 >> 0x1f ^ param_1) - ((int)param_1 >> 0x1f);
    iVar2 = FUN_ram_0001e4f8(uVar1);
    uVar4 = 0x9e - iVar2;
    param_1 = param_1 >> 0x1f;
    if ((int)uVar4 < 0x97) {
      uVar4 = uVar4 & 0xff;
      if (8 < iVar2) {
        uVar1 = uVar1 << (iVar2 - 8U & 0x1f);
      }
      uVar1 = uVar1 & 0x7fffff;
    }
    else {
      if (0x99 < (int)uVar4) {
        uVar1 = uVar1 >> (5U - iVar2 & 0x1f) | (uint)(uVar1 << (iVar2 + 0x1bU & 0x1f) != 0);
      }
      if (5 < iVar2) {
        uVar1 = uVar1 << (iVar2 - 5U & 0x1f);
      }
      uVar3 = uVar1 & 0xfbffffff;
      if ((((uVar1 & 7) != 0) && ((uVar1 & 0xf) != 4)) &&
         (uVar3 = uVar3 + 4, (int)(uVar3 * 0x20) < 0)) {
        uVar3 = uVar3 & 0xfbffffff;
        uVar4 = 0x9f - iVar2;
      }
      uVar1 = (uVar3 & 0x3ffffff) >> 3;
      uVar4 = uVar4 & 0xff;
    }
  }
  return uVar1 | uVar4 << 0x17 | param_1 << 0x1f;
}

