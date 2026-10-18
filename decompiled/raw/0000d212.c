/* Original address: 0x0000d212; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC 8MHz HSE times 18 PLL configuration for 144MHz */

void SystemInit(void)

{
  uint32_t uVar1;
  int iStack_8;
  
  gp = &__global_pointer;
  uVar1 = RCC.CTLR;
  RCC.CTLR = uVar1 | 1;
  uVar1 = RCC.CFGR0;
  RCC.CFGR0 = uVar1 & 0xf0ff0000;
  uVar1 = RCC.CTLR;
  RCC.CTLR = uVar1 & 0xfef6ffff;
  uVar1 = RCC.CTLR;
  RCC.CTLR = uVar1 & 0xfffbffff;
  uVar1 = RCC.CFGR0;
  RCC.CFGR0 = uVar1 & 0xff80ffff;
  RCC.INTR = 0x9f0000;
  iStack_8 = 0;
  uVar1 = RCC.CTLR;
  RCC.CTLR = uVar1 | 0x10000;
  do {
    uVar1 = RCC.CTLR;
    iStack_8 = iStack_8 + 1;
    if ((uVar1 & 0x20000) != 0) break;
  } while (iStack_8 != 0x1000);
  uVar1 = RCC.CTLR;
  if ((int)(uVar1 << 0xe) < 0) {
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1;
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1;
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1 | 0x400;
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1 & 0xffc0ffff;
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1 | 0x3d0000;
    uVar1 = RCC.CTLR;
    RCC.CTLR = uVar1 | 0x1000000;
    do {
      uVar1 = RCC.CTLR;
    } while (-1 < (int)(uVar1 << 6));
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1 & 0xfffffffc;
    uVar1 = RCC.CFGR0;
    RCC.CFGR0 = uVar1 | 2;
    do {
      uVar1 = RCC.CFGR0;
    } while ((uVar1 & 0xc) != 8);
  }
  return;
}

