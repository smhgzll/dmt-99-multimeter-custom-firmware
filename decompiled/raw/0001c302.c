/* Original address: 0x0001c302; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Type propagation algorithm not settling */

undefined4 FUN_ram_0001c302(uint param_1,uint param_2,uint param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  uVar3 = param_2 >> 0x14 & 0x7ff;
  uVar6 = param_2 & 0xfffff;
  uVar4 = param_4 & 0xfffff;
  iVar1 = -((int)param_2 >> 0x1f);
  uVar5 = param_4 >> 0x14 & 0x7ff;
  iVar2 = -((int)param_4 >> 0x1f);
  if (uVar3 == 0x7ff) {
    if (uVar6 != 0 || param_1 != 0) {
      return 2;
    }
    if (uVar5 != 0x7ff) goto LAB_ram_0001c374;
    if (param_3 != 0 || uVar4 != 0) {
      return 2;
    }
LAB_ram_0001c37c:
    if ((iVar1 != iVar2) || (uVar5 < uVar3)) goto LAB_ram_0001c35e;
  }
  else {
    if ((uVar5 == 0x7ff) && (uVar4 != 0 || param_3 != 0)) {
      return 2;
    }
    if (uVar3 != 0) {
LAB_ram_0001c374:
      if ((uVar5 == 0) && (param_3 == 0 && uVar4 == 0)) goto LAB_ram_0001c35e;
      goto LAB_ram_0001c37c;
    }
    if ((uVar5 == 0) && (param_3 == 0 && uVar4 == 0)) {
      if (uVar6 == 0 && param_1 == 0) {
        return 0;
      }
      goto LAB_ram_0001c35e;
    }
    if (uVar6 == 0 && param_1 == 0) {
      if (iVar2 != 0) {
        return 1;
      }
      return 0xffffffff;
    }
    if (iVar1 != iVar2) goto LAB_ram_0001c35e;
  }
  if (uVar5 <= uVar3) {
    if (uVar4 < uVar6) {
LAB_ram_0001c35e:
      if (iVar1 != 0) {
        return 0xffffffff;
      }
      return 1;
    }
    if (uVar6 == uVar4) {
      if (param_3 < param_1) goto LAB_ram_0001c35e;
      if (param_3 <= param_1) {
        return 0;
      }
    }
    else if (uVar4 <= uVar6) {
      return 0;
    }
  }
  if (iVar1 == 0) {
    return 0xffffffff;
  }
  return 1;
}

