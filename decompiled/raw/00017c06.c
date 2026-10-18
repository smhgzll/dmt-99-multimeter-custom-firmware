/* Original address: 0x00017c06; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00017c06(uint param_1,uint param_2,int param_3,int param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  gp = &__global_pointer;
  iVar1 = param_3 - param_1;
  iVar2 = param_4 - param_2;
  iVar7 = 1;
  if ((iVar1 < 1) && (iVar7 = 0, iVar1 != 0)) {
    iVar1 = param_1 - param_3;
    iVar7 = -1;
  }
  iVar8 = 1;
  if ((iVar2 < 1) && (iVar8 = 0, iVar2 != 0)) {
    iVar2 = param_2 - param_4;
    iVar8 = -1;
  }
  iVar3 = iVar1;
  if (iVar1 < iVar2) {
    iVar3 = iVar2;
  }
  iVar6 = 0;
  iVar5 = 0;
  for (uVar4 = 0; (int)uVar4 <= iVar3 + 1; uVar4 = uVar4 + 1 & 0xffff) {
    iVar5 = iVar5 + iVar1;
    FUN_ram_00017ab2(param_1 & 0xffff,param_2 & 0xffff,param_5);
    iVar6 = iVar6 + iVar2;
    if (iVar3 < iVar5) {
      iVar5 = iVar5 - iVar3;
      param_1 = param_1 + iVar7;
    }
    if (iVar3 < iVar6) {
      iVar6 = iVar6 - iVar3;
      param_2 = param_2 + iVar8;
    }
  }
  return;
}

