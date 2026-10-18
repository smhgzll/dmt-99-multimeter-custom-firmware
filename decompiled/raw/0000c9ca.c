/* Original address: 0x0000c9ca; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Polls internal flash status with timeout */

int FLASH_WaitForLastOperation(int param_1)

{
  int iVar1;
  
  gp = &__global_pointer;
  iVar1 = FLASH_GetStatus();
  while (iVar1 == 1) {
    if (param_1 == 0) goto LAB_ram_0000c9dc;
    iVar1 = FLASH_GetStatus();
    param_1 = param_1 + -1;
  }
  if (param_1 == 0) {
LAB_ram_0000c9dc:
    iVar1 = 5;
  }
  return iVar1;
}

