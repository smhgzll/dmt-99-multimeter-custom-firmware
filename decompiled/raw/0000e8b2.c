/* Original address: 0x0000e8b2; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

uint FUN_ram_0000e8b2(int *param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  
  gp = &__global_pointer;
  iVar2 = *param_1;
  if (param_2 == 0) {
    uVar5 = *(uint *)(iVar2 + 0x14);
    if ((uVar5 == 0) || (*(uint *)(iVar2 + 0x1c) <= uVar5)) {
      uVar5 = 1;
    }
  }
  else {
    uVar3 = FUN_ram_0000e71e(iVar2);
    if (uVar3 < 2) {
      return 1;
    }
    if (uVar3 == 0xffffffff) {
      return 0xffffffff;
    }
    uVar5 = param_2;
    if (uVar3 < *(uint *)(iVar2 + 0x1c)) {
      return uVar3;
    }
  }
  if (*(int *)(iVar2 + 0x18) != 0) {
    uVar3 = uVar5;
    if (uVar5 == param_2) {
      uVar1 = uVar5 + 1;
      if (*(uint *)(iVar2 + 0x1c) <= uVar1) {
        uVar1 = 2;
      }
      iVar4 = FUN_ram_0000e71e(*param_1,uVar1);
      if (iVar4 == 1) {
        return 1;
      }
      if (iVar4 == -1) {
        return 0xffffffff;
      }
      if (iVar4 == 0) {
        if (uVar1 != 0) {
LAB_ram_0000e962:
          iVar4 = FUN_ram_0000e5e8(iVar2,uVar1,0xffffffff);
          if ((iVar4 == 0) &&
             ((param_2 == 0 || (iVar4 = FUN_ram_0000e5e8(iVar2,param_2,uVar1), iVar4 == 0)))) {
            *(uint *)(iVar2 + 0x14) = uVar1;
            if (*(uint *)(iVar2 + 0x18) <= *(int *)(iVar2 + 0x1c) - 2U) {
              *(uint *)(iVar2 + 0x18) = *(uint *)(iVar2 + 0x18) - 1;
            }
            *(byte *)(iVar2 + 4) = *(byte *)(iVar2 + 4) | 1;
            return uVar1;
          }
          if (iVar4 == 1) {
            return 0xffffffff;
          }
          return 1;
        }
      }
      else {
        uVar1 = *(uint *)(iVar2 + 0x14);
        if ((1 < uVar1) && (uVar1 < *(uint *)(iVar2 + 0x1c))) {
          uVar5 = uVar1;
          uVar3 = uVar1;
        }
      }
    }
    do {
      uVar1 = uVar5 + 1;
      if (*(uint *)(iVar2 + 0x1c) <= uVar1) {
        if (uVar3 < 2) {
          return 0;
        }
        uVar1 = 2;
      }
      iVar4 = FUN_ram_0000e71e(*param_1,uVar1);
      if (iVar4 == 0) goto LAB_ram_0000e962;
      if (iVar4 == 1) {
        return 1;
      }
      if (iVar4 == -1) {
        return 0xffffffff;
      }
      uVar5 = uVar1;
    } while (uVar1 != uVar3);
  }
  return 0;
}

