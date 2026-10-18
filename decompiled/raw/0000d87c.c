/* Original address: 0x0000d87c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* high: Erases/programs 0x08040000 and copies 128 halfwords */

void settings_commit_internal_flash(void)

{
  uint32_t uVar1;
  uint uVar2;
  undefined2 *puVar3;
  
  gp = &__global_pointer;
  uVar1 = RCC.CFGR0;
  RCC.CFGR0 = uVar1 | 0x80;
  _DAT_csreg_0800 = 0x6000;
  FLASH_Unlock();
  DAT_ram_20000220 = 1;
  FLASH_ClearFlag(0x31);
  FLASH_Unlock_Fast();
  FLASH_ErasePage_Fast(0x8040000);
  FLASH_Lock_Fast();
  DAT_ram_2000021c = 0x8040000;
  uVar2 = 0;
  while ((DAT_ram_2000021c < 0x8040100 && (DAT_ram_200001d0 == 4))) {
    puVar3 = &DAT_ram_20000020 + uVar2;
    uVar2 = uVar2 + 1 & 0xff;
    DAT_ram_200001d0 = FLASH_ProgramHalfWord(DAT_ram_2000021c,*puVar3);
    DAT_ram_2000021c = DAT_ram_2000021c + 2;
  }
  FLASH_Lock();
  return;
}

