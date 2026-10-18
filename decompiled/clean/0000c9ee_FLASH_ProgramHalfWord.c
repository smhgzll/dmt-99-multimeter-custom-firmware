/* Semantic review: high. Internal flash program bit halfword store wait and disable.
 * Address 0x0000c9ee; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c9ee; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Internal flash program bit halfword store wait and disable */

void FLASH_ProgramHalfWord(undefined2 *param_1,undefined2 param_2)

{
  uint uVar1;
  int iVar2;
  
  gp = &__global_pointer;
  iVar2 = FLASH_WaitForLastOperation(0x5000);
  if (iVar2 == 4) {
    uVar1 = DAT_ram_40022010;
    DAT_ram_40022010 = uVar1 | 1;
    *param_1 = param_2;
    FLASH_WaitForLastOperation(0x5000);
    uVar1 = DAT_ram_40022010;
    DAT_ram_40022010 = uVar1 & 0xfffffffe;
  }
  return;
}

