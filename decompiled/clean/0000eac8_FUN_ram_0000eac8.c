/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000eac8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000eac8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

bool FUN_ram_0000eac8(int param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  gp = &__global_pointer;
  if ((*(char *)(param_1 + 3) != '\0') && (iVar2 = FUN_ram_0000e134(), iVar2 != 0)) {
    gp = &__global_pointer;
    return true;
  }
  iVar2 = FUN_ram_0000de84(param_1,param_2);
  *(int *)(param_1 + 0x34) = iVar2;
  memset_recovered(param_1 + 0x38,0,0x1000);
  uVar1 = 0;
  while ((uVar1 < *(ushort *)(param_1 + 10) &&
         (iVar3 = disk_write(*(undefined1 *)(param_1 + 1),param_1 + 0x38,iVar2 + uVar1,1),
         iVar3 == 0))) {
    uVar1 = uVar1 + 1;
  }
  return *(ushort *)(param_1 + 10) != uVar1;
}

