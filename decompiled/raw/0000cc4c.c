/* Original address: 0x0000cc4c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* high: RCC clock mux PLL multiplier and bus prescaler calculations */

void RCC_GetClocksFreq(uint *param_1)

{
  bool bVar1;
  uint32_t uVar2;
  uint32_t uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  gp = &__global_pointer;
  uVar2 = RCC.CFGR0;
  if (((uVar2 & 0xc) == 4) || ((uVar2 & 0xc) != 8)) {
    uVar5 = 8000000;
  }
  else {
    uVar2 = RCC.CFGR0;
    uVar3 = RCC.CFGR0;
    iVar6 = (uVar2 >> 0x12 & 0xf) + 2;
    if (_DAT_ram_1ffff70c << 0x11 < 0) {
      if (iVar6 == 2) goto LAB_ram_0000cce6;
      if (iVar6 == 0xf) {
        bVar1 = true;
        iVar6 = 0xd;
      }
      else if (iVar6 == 0x10) {
        bVar1 = false;
        iVar6 = 0xf;
      }
      else {
        bVar1 = false;
        if (iVar6 == 0x11) {
          iVar6 = 0x10;
        }
      }
    }
    else {
      bVar1 = false;
      if (iVar6 == 0x11) {
LAB_ram_0000cce6:
        bVar1 = false;
        iVar6 = 0x12;
      }
    }
    if ((uVar3 & 0x10000) == 0) {
      uVar5 = EXTEN;
      if ((uVar5 & 0x10) == 0) goto LAB_ram_0000cd3c;
LAB_ram_0000cd14:
      iVar4 = 8000000;
    }
    else {
      uVar2 = RCC.CFGR0;
      if (-1 < (int)(uVar2 << 0xe)) goto LAB_ram_0000cd14;
LAB_ram_0000cd3c:
      iVar4 = 4000000;
    }
    *param_1 = iVar6 * iVar4;
    if (!bVar1) goto LAB_ram_0000cc6a;
    uVar5 = *param_1 >> 1;
  }
  *param_1 = uVar5;
LAB_ram_0000cc6a:
  uVar2 = RCC.CFGR0;
  uVar5 = *param_1 >> (*(byte *)((uVar2 >> 4 & 0xf) + 0x20000000) & 0x1f);
  param_1[1] = uVar5;
  uVar2 = RCC.CFGR0;
  param_1[2] = uVar5 >> (*(byte *)((uVar2 >> 8 & 7) + 0x20000000) & 0x1f);
  uVar2 = RCC.CFGR0;
  uVar5 = uVar5 >> (*(byte *)((uVar2 >> 0xb & 7) + 0x20000000) & 0x1f);
  param_1[3] = uVar5;
  uVar2 = RCC.CFGR0;
  param_1[4] = uVar5 / (byte)(&DAT_ram_200001c8)[uVar2 >> 0xe & 3];
  return;
}

