/* Original address: 0x0000df10; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000df10(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  uint uVar2;
  char *pcVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  if (param_1 == (undefined4 *)0x0) {
    *param_2 = 0;
    return 9;
  }
  pcVar3 = (char *)*param_1;
  if ((pcVar3 != (char *)0x0) && (*pcVar3 != '\0')) {
    if (*(short *)(param_1 + 1) == *(short *)(pcVar3 + 6)) {
      uVar2 = disk_status(pcVar3[1]);
      if ((uVar2 & 1) == 0) {
        uVar4 = *param_1;
        uVar1 = 0;
        goto LAB_ram_0000df1e;
      }
    }
  }
  uVar1 = 9;
  uVar4 = 0;
LAB_ram_0000df1e:
  *param_2 = uVar4;
  return uVar1;
}

