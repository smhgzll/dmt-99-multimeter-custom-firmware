/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000e17c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000e17c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

uint FUN_ram_0000e17c(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  if (*(int *)(param_1 + 0x34) != param_2) {
    if ((*(char *)(param_1 + 3) == '\0') || (uVar2 = FUN_ram_0000e134(), uVar2 == 0)) {
      iVar1 = disk_read(*(undefined1 *)(param_1 + 1),param_1 + 0x38,param_2,1);
      if (iVar1 != 0) {
        param_2 = -1;
      }
      uVar2 = (uint)(iVar1 != 0);
      *(int *)(param_1 + 0x34) = param_2;
    }
    return uVar2;
  }
  return 0;
}

