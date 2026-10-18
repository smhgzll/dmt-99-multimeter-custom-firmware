/* Original address: 0x0000eb26; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

undefined4 FUN_ram_0000eb26(int *param_1,int param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  gp = &__global_pointer;
  iVar4 = *param_1;
  uVar5 = param_1[4] + 0x20;
  if (0x1fffff < uVar5) {
    param_1[6] = 0;
  }
  if (param_1[6] == 0) {
    gp = &__global_pointer;
    return 4;
  }
  uVar1 = *(ushort *)(iVar4 + 0xc);
  if (uVar5 % (uint)uVar1 == 0) {
    param_1[6] = param_1[6] + 1;
    if (param_1[5] == 0) {
      if ((uint)*(ushort *)(iVar4 + 8) <= uVar5 >> 5) {
LAB_ram_0000eb68:
        param_1[6] = 0;
        gp = &__global_pointer;
        return 4;
      }
    }
    else if ((*(ushort *)(iVar4 + 10) - 1 & uVar5 / uVar1) == 0) {
      uVar2 = FUN_ram_0000e71e(iVar4);
      if (uVar2 < 2) {
LAB_ram_0000eb6e:
        gp = &__global_pointer;
        return 2;
      }
      if (uVar2 == 0xffffffff) {
LAB_ram_0000eb6e:
        gp = &__global_pointer;
        return 1;
      }
      if (*(uint *)(iVar4 + 0x1c) <= uVar2) {
        if (param_2 == 0) goto LAB_ram_0000eb68;
        uVar2 = FUN_ram_0000e8b2(param_1,param_1[5]);
        if (uVar2 == 0) {
          gp = &__global_pointer;
          return 7;
        }
        if (uVar2 == 1) goto LAB_ram_0000eb6e;
        if ((uVar2 == 0xffffffff) || (iVar3 = FUN_ram_0000eac8(iVar4,uVar2), iVar3 != 0))
        goto LAB_ram_0000eb6e;
      }
      param_1[5] = uVar2;
      iVar3 = FUN_ram_0000de84(iVar4,uVar2);
      param_1[6] = iVar3;
    }
  }
  uVar1 = *(ushort *)(iVar4 + 0xc);
  param_1[4] = uVar5;
  param_1[7] = uVar5 % (uint)uVar1 + iVar4 + 0x38;
  return 0;
}

