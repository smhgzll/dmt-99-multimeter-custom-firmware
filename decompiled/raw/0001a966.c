/* Original address: 0x0001a966; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001a966(undefined4 param_1,int param_2,int param_3,uint param_4,int param_5,int param_6
                    ,uint param_7,int param_8,char param_9)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  uint uVar7;
  undefined8 uVar8;
  undefined2 uStack_58;
  undefined1 uStack_56;
  char acStack_43 [15];
  
  gp = &__global_pointer;
  if (param_2 == 0 && param_3 == 0) {
    if (param_9 == '\0') {
      uStack_58 = 0x30;
    }
    else {
      uStack_58 = 0x302d;
      uStack_56 = 0;
    }
    iVar3 = FUN_ram_0001a794(param_1,&uStack_58,param_6,param_7,0);
  }
  else {
    iVar4 = 0;
    if (((param_5 != 0) && (iVar4 = 0, param_4 == 10)) && (param_3 < 0)) {
      param_2 = -param_2;
      param_3 = -(uint)(param_2 != 0) - param_3;
      iVar4 = 1;
    }
    uVar8 = CONCAT44(param_3,param_2);
    acStack_43[2] = 0;
    uVar7 = (int)param_4 >> 0x1f;
    pcVar5 = acStack_43 + 2;
    do {
      pcVar6 = pcVar5;
      uVar1 = (uint)((ulonglong)uVar8 >> 0x20);
      uVar2 = (uint)uVar8;
      iVar3 = FUN_ram_0001b294(uVar2,uVar1,param_4,uVar7);
      if (9 < iVar3) {
        iVar3 = iVar3 + param_8 + -0x3a;
      }
      pcVar6[-1] = (char)iVar3 + '0';
      pcVar5 = pcVar6 + -1;
      uVar8 = FUN_ram_0001af02(uVar2,uVar1,param_4,uVar7);
    } while ((uVar7 <= uVar1) && ((uVar7 != uVar1 || (param_4 <= uVar2))));
    if (iVar4 != 0) {
      if ((param_6 == 0) || ((param_7 & 2) == 0)) {
        pcVar6[-2] = '-';
        iVar4 = 0;
        pcVar5 = pcVar6 + -2;
      }
      else {
        FUN_ram_0001a74a(param_1,0x2d);
        param_6 = param_6 + -1;
      }
    }
    iVar3 = FUN_ram_0001a794(param_1,pcVar5,param_6,param_7,0);
    iVar3 = iVar3 + iVar4;
  }
  return iVar3;
}

