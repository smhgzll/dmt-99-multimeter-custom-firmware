/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000e9a4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000e9a4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

int FUN_ram_0000e9a4(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  gp = &__global_pointer;
  iVar1 = *param_1;
  if ((param_2 < 2) || (*(uint *)(iVar1 + 0x1c) <= param_2)) {
LAB_ram_0000ea16:
    iVar3 = 2;
  }
  else if ((param_3 == 0) || (iVar3 = FUN_ram_0000e5e8(iVar1,param_3,0xffffffff), iVar3 == 0)) {
    do {
      uVar2 = FUN_ram_0000e71e(*param_1,param_2);
      if (uVar2 == 0) break;
      if (uVar2 == 1) goto LAB_ram_0000ea16;
      if (uVar2 == 0xffffffff) {
        return 1;
      }
      iVar3 = FUN_ram_0000e5e8(iVar1,param_2,0);
      if (iVar3 != 0) {
        return iVar3;
      }
      if (*(uint *)(iVar1 + 0x18) < *(uint *)(iVar1 + 0x1c) - 2) {
        *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) + 1;
        *(byte *)(iVar1 + 4) = *(byte *)(iVar1 + 4) | 1;
      }
      param_2 = uVar2;
    } while (uVar2 < *(uint *)(iVar1 + 0x1c));
    iVar3 = 0;
  }
  return iVar3;
}

