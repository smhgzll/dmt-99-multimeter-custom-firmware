/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000ed8e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ed8e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

int FUN_ram_0000ed8e(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  iVar1 = *param_1;
  uVar3 = param_1[4];
  if ((param_1[0xb] == -1) || (iVar2 = FUN_ram_0000e7fc(), iVar2 == 0)) {
    do {
      iVar2 = FUN_ram_0000e17c(iVar1,param_1[6]);
      if (iVar2 != 0) break;
      *(undefined1 *)param_1[7] = 0xe5;
      *(undefined1 *)(iVar1 + 3) = 1;
      if (uVar3 <= (uint)param_1[4]) {
        return 0;
      }
      iVar2 = FUN_ram_0000eb26(param_1,0);
    } while (iVar2 == 0);
    if (iVar2 == 4) {
      iVar2 = 2;
    }
  }
  return iVar2;
}

