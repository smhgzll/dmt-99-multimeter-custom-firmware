/* Original address: 0x0000d2fc; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC decode stores core clock at 0x200001cc */

void SystemCoreClockUpdate(void)

{
  uint32_t uVar1;
  uint32_t uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  gp = &__global_pointer;
  uVar1 = RCC.CFGR0;
  if (((uVar1 & 0xc) == 4) || ((uVar1 & 0xc) != 8)) {
    uVar4 = 8000000;
    goto LAB_ram_0000d31c;
  }
  uVar1 = RCC.CFGR0;
  uVar2 = RCC.CFGR0;
  iVar5 = (uVar1 >> 0x12 & 0xf) + 2;
  if (iVar5 == 0x11) {
    iVar5 = 0x12;
  }
  if ((uVar2 & 0x10000) == 0) {
    uVar4 = EXTEN;
    if ((uVar4 & 0x10) != 0) goto LAB_ram_0000d364;
LAB_ram_0000d37c:
    iVar3 = 4000000;
  }
  else {
    uVar1 = RCC.CFGR0;
    if ((int)(uVar1 << 0xe) < 0) goto LAB_ram_0000d37c;
LAB_ram_0000d364:
    iVar3 = 8000000;
  }
  uVar4 = iVar5 * iVar3;
LAB_ram_0000d31c:
  uVar1 = RCC.CFGR0;
  system_core_clock_hz = uVar4 >> ((byte)(&DAT_ram_20000010)[uVar1 >> 4 & 0xf] & 0x1f);
  return;
}

