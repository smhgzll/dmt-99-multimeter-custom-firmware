/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000ebf0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ebf0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

int FUN_ram_0000ebf0(undefined4 *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  gp = &__global_pointer;
  uVar3 = *param_1;
  iVar2 = FUN_ram_0000e7fc(param_1,0);
  if (iVar2 == 0) {
    iVar1 = 0;
    do {
      iVar2 = FUN_ram_0000e17c(uVar3,param_1[6]);
      if (iVar2 != 0) break;
      if ((*(char *)param_1[7] == -0x1b) || (*(char *)param_1[7] == '\0')) {
        iVar1 = iVar1 + 1;
        if (iVar1 == param_2) {
          gp = &__global_pointer;
          return 0;
        }
      }
      else {
        iVar1 = 0;
      }
      iVar2 = FUN_ram_0000eb26(param_1,1);
    } while (iVar2 == 0);
  }
  if (iVar2 == 4) {
    iVar2 = 7;
  }
  return iVar2;
}

