/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001a874; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001a874; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001a874(undefined4 param_1,uint param_2,uint param_3,int param_4,int param_5,
                    uint param_6,int param_7,undefined4 param_8)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  undefined2 auStack_38 [10];
  char acStack_23 [7];
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    auStack_38[0] = 0x30;
    iVar3 = FUN_ram_0001a794(param_1,auStack_38,param_5,param_6,0);
  }
  else {
    iVar2 = 0;
    if (((param_4 != 0) && (iVar2 = 0, param_3 == 10)) && ((int)param_2 < 0)) {
      param_2 = -param_2;
      iVar2 = 1;
    }
    acStack_23[2] = 0;
    pcVar7 = acStack_23 + 2;
    do {
      pcVar6 = pcVar7;
      uVar5 = param_2 % param_3;
      if (9 < (int)uVar5) {
        uVar5 = uVar5 + param_7 + -0x3a;
      }
      pcVar6[-1] = (char)uVar5 + '0';
      pcVar7 = pcVar6 + -1;
      bVar1 = param_3 <= param_2;
      param_2 = param_2 / param_3;
    } while (bVar1);
    if (iVar2 != 0) {
      if ((param_5 == 0) || ((param_6 & 2) == 0)) {
        pcVar6[-2] = '-';
        iVar2 = 0;
        pcVar7 = pcVar6 + -2;
      }
      else {
        FUN_ram_0001a74a(param_1,0x2d);
        param_5 = param_5 + -1;
      }
    }
    if ((param_6 & 4) != 0) {
      if (param_7 == 0x61) {
        FUN_ram_0001a74a(param_1,0x30);
        uVar4 = 0x78;
      }
      else {
        FUN_ram_0001a74a(param_1,0x30);
        uVar4 = 0x58;
      }
      FUN_ram_0001a74a(param_1,uVar4);
      iVar2 = iVar2 + 2;
      param_5 = param_5 + -2;
    }
    iVar3 = FUN_ram_0001a794(param_1,pcVar7,param_5,param_6,param_8);
    iVar3 = iVar3 + iVar2;
  }
  return iVar3;
}

