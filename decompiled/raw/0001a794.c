/* Original address: 0x0001a794; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001a794(undefined4 param_1,int param_2,int param_3,uint param_4,int param_5)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  gp = &__global_pointer;
  iVar1 = 0;
  uVar3 = 0x20;
  if (-1 < param_3) {
    for (; *(char *)(param_2 + iVar1) != '\0'; iVar1 = iVar1 + 1) {
    }
    if (iVar1 < param_3) {
      if (iVar1 < param_5) {
        param_3 = param_3 - param_5;
      }
      else {
        param_3 = param_3 - iVar1;
      }
    }
    else {
      param_3 = 0;
    }
    uVar3 = 0x20;
    if (((param_4 & 2) != 0) && (param_5 == 0)) {
      uVar3 = 0x30;
    }
  }
  iVar2 = 0;
  iVar4 = param_3;
  if ((param_4 & 1) == 0) {
    for (; 0 < iVar4; iVar4 = iVar4 + -1) {
      FUN_ram_0001a74a(param_1,uVar3);
    }
    iVar2 = param_3;
    if (param_3 < 0) {
      iVar2 = 0;
    }
    param_3 = param_3 - iVar2;
  }
  iVar4 = iVar2;
  if (iVar1 < param_5) {
    iVar4 = param_5 - iVar1;
    do {
      iVar4 = iVar4 + -1;
      FUN_ram_0001a74a(param_1,0x30);
    } while (iVar4 != 0);
    iVar2 = iVar2 + (param_5 - iVar1);
    iVar4 = iVar2;
  }
  for (; iVar1 = param_3, *(char *)((iVar2 - iVar4) + param_2) != '\0'; iVar2 = iVar2 + 1) {
    FUN_ram_0001a74a(param_1);
  }
  for (; 0 < iVar1; iVar1 = iVar1 + -1) {
    FUN_ram_0001a74a(param_1,uVar3);
  }
  if (param_3 < 0) {
    param_3 = 0;
  }
  return param_3 + iVar2;
}

