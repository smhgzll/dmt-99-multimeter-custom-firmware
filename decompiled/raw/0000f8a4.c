/* Original address: 0x0000f8a4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

int FUN_ram_0000f8a4(undefined4 *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iStack_14;
  
  gp = &__global_pointer;
  iVar1 = FUN_ram_0000df10(param_1,&iStack_14);
  if ((iVar1 == 0) && ((*(byte *)(param_1 + 4) & 0x40) != 0)) {
    if ((char)*(byte *)(param_1 + 4) < '\0') {
      iVar1 = disk_write(*(undefined1 *)(iStack_14 + 1),param_1 + 10,param_1[7],1);
      if (iVar1 != 0) {
        gp = &__global_pointer;
        return 1;
      }
      *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) & 0x7f;
    }
    uVar2 = FUN_ram_0000dd48();
    iVar1 = FUN_ram_0000e17c(iStack_14,param_1[8]);
    if (iVar1 == 0) {
      iVar1 = param_1[9];
      *(byte *)(iVar1 + 0xb) = *(byte *)(iVar1 + 0xb) | 0x20;
      FUN_ram_0000e116(*param_1,iVar1,param_1[2]);
      FUN_ram_0000de70(iVar1 + 0x1c,param_1[3]);
      FUN_ram_0000de70(iVar1 + 0x16,uVar2);
      *(undefined1 *)(iVar1 + 0x12) = 0;
      *(undefined1 *)(iVar1 + 0x13) = 0;
      *(undefined1 *)(iStack_14 + 3) = 1;
      iVar1 = FUN_ram_0000ea1e();
      *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) & 0xbf;
    }
  }
  return iVar1;
}

