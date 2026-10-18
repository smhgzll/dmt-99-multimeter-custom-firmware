/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00006854; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00006854; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

int FUN_ram_00006854(int *param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  
  gp = &__global_pointer;
  pbVar6 = (byte *)param_1[1];
  uVar5 = (uint)*(byte *)(param_1 + 3);
  iVar3 = *param_1;
  uVar4 = (uint)*pbVar6;
  bVar1 = false;
  iVar2 = 0;
  do {
    if (uVar5 == 0) {
      while( true ) {
        if (iVar3 == 0) {
          pbVar6 = (byte *)param_1[2];
          iVar3 = (*(code *)param_1[0x1d])(param_1,pbVar6,0x200);
          if (iVar3 == 0) {
            gp = &__global_pointer;
            return -2;
          }
        }
        else {
          pbVar6 = pbVar6 + 1;
        }
        iVar3 = iVar3 + -1;
        uVar4 = (uint)*pbVar6;
        if (bVar1) break;
        if (uVar4 != 0xff) goto LAB_ram_000068aa;
        bVar1 = true;
      }
      if (uVar4 != 0) {
        gp = &__global_pointer;
        return -6;
      }
      *pbVar6 = 0xff;
      bVar1 = false;
      uVar4 = 0xff;
LAB_ram_000068aa:
      uVar5 = 0x80;
    }
    iVar2 = iVar2 * 2;
    if ((uVar5 & uVar4) != 0) {
      iVar2 = iVar2 + 1;
    }
    param_2 = param_2 + -1;
    uVar5 = uVar5 >> 1;
    if (param_2 == 0) {
      *param_1 = iVar3;
      *(char *)(param_1 + 3) = (char)uVar5;
      param_1[1] = (int)pbVar6;
      return iVar2;
    }
  } while( true );
}

