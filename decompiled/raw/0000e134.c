/* Original address: 0x0000e134; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000e134(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  gp = &__global_pointer;
  iVar2 = disk_write(*(undefined1 *)(param_1 + 1),param_1 + 0x38,*(undefined4 *)(param_1 + 0x34),1);
  uVar1 = 1;
  if (iVar2 == 0) {
    *(undefined1 *)(param_1 + 3) = 0;
    uVar1 = 0;
    if (((uint)(*(int *)(param_1 + 0x34) - *(int *)(param_1 + 0x28)) < *(uint *)(param_1 + 0x20)) &&
       (*(char *)(param_1 + 2) == '\x02')) {
      disk_write(*(undefined1 *)(param_1 + 1),param_1 + 0x38,
                 *(int *)(param_1 + 0x34) + *(uint *)(param_1 + 0x20),1);
    }
  }
  return uVar1;
}

