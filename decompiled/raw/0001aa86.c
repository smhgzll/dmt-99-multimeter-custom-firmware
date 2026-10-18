/* Original address: 0x0001aa86; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001aa86(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                    undefined4 param_5,int param_6,undefined4 param_7,undefined4 param_8)

{
  ulonglong uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  
  gp = &__global_pointer;
  uVar10 = FUN_ram_0001d14c(param_2,param_3);
  uVar5 = (undefined4)((ulonglong)uVar10 >> 0x20);
  uVar3 = FUN_ram_0001c302(param_2,param_3,0,0);
  iVar4 = FUN_ram_0001a966(param_1,(int)uVar10,uVar5,param_4,1,param_5,param_7,param_8,uVar3 >> 0x1f
                          );
  FUN_ram_0001a74a(param_1,0x2e);
  uVar3 = 1;
  iVar2 = 0;
  for (iVar9 = 0; iVar9 < param_6; iVar9 = iVar9 + 1) {
    uVar1 = (ulonglong)uVar3;
    uVar3 = uVar3 * 10;
    iVar2 = (int)(uVar1 * 10 >> 0x20) + iVar2 * 10;
  }
  uVar10 = FUN_ram_0001d208((int)uVar10,uVar5);
  uVar10 = FUN_ram_0001c870(param_2,param_3,(int)uVar10,(int)((ulonglong)uVar10 >> 0x20));
  uVar11 = FUN_ram_0001d208(uVar3,iVar2);
  FUN_ram_0001c3b8((int)uVar10,(int)((ulonglong)uVar10 >> 0x20),(int)uVar11,
                   (int)((ulonglong)uVar11 >> 0x20));
  uVar10 = FUN_ram_0001d14c();
  uVar6 = (uint)((ulonglong)uVar10 >> 0x20);
  uVar8 = (int)uVar6 >> 0x1f;
  uVar3 = (uint)uVar10 ^ uVar8;
  uVar7 = uVar3 - uVar8;
  iVar2 = FUN_ram_0001a966(param_1,uVar7,((uVar8 ^ uVar6) - uVar8) - (uint)(uVar3 < uVar7),param_4,1
                           ,param_6,2,param_8,0);
  return iVar2 + iVar4 + 1;
}

