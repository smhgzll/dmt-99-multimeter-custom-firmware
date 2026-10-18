/* Original address: 0x000068d2; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

uint FUN_ram_000068d2(int *param_1,byte *param_2,ushort *param_3,byte *param_4)

{
  bool bVar1;
  byte *pbVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  
  gp = &__global_pointer;
  pbVar2 = (byte *)param_1[1];
  uVar6 = (uint)*(byte *)(param_1 + 3);
  iVar3 = *param_1;
  uVar5 = (uint)*pbVar2;
  iVar8 = 0x10;
  bVar1 = false;
  uVar7 = 0;
  do {
    if (uVar6 == 0) {
      while( true ) {
        if (iVar3 == 0) {
          pbVar2 = (byte *)param_1[2];
          iVar3 = (*(code *)param_1[0x1d])(param_1,pbVar2,0x200,param_4,uVar5,(code *)param_1[0x1d])
          ;
          if (iVar3 == 0) {
            gp = &__global_pointer;
            return 0xfffffffe;
          }
        }
        else {
          pbVar2 = pbVar2 + 1;
        }
        iVar3 = iVar3 + -1;
        uVar5 = (uint)*pbVar2;
        if (bVar1) break;
        if (uVar5 != 0xff) goto LAB_ram_00006930;
        bVar1 = true;
      }
      if (uVar5 != 0) {
        return 0xfffffffa;
      }
      *pbVar2 = 0xff;
      bVar1 = false;
      uVar5 = 0xff;
LAB_ram_00006930:
      uVar6 = 0x80;
    }
    uVar7 = uVar7 * 2;
    if ((uVar6 & uVar5) != 0) {
      uVar7 = uVar7 + 1;
    }
    uVar6 = uVar6 >> 1;
    pbVar4 = param_4 + *param_2;
    for (; param_4 != pbVar4; param_4 = param_4 + 1) {
      if (*param_3 == uVar7) {
        *(char *)(param_1 + 3) = (char)uVar6;
        *param_1 = iVar3;
        param_1[1] = (int)pbVar2;
        gp = &__global_pointer;
        return (uint)*param_4;
      }
      param_3 = param_3 + 1;
    }
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) {
      return 0xfffffffa;
    }
    param_2 = param_2 + 1;
  } while( true );
}

