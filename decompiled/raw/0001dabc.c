/* Original address: 0x0001dabc; Ghidra pseudocode, not compilable reconstructed source. */

undefined4 FUN_ram_0001dabc(uint param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  uVar3 = param_1 >> 0x17 & 0xff;
  uVar6 = param_1 & 0x7fffff;
  iVar4 = -((int)param_1 >> 0x1f);
  uVar5 = param_2 & 0x7fffff;
  uVar2 = param_2 >> 0x17 & 0xff;
  iVar1 = -((int)param_2 >> 0x1f);
  if (uVar3 == 0xff) {
    if (uVar6 != 0) {
      return 2;
    }
    if (uVar2 != 0xff) goto LAB_ram_0001db1a;
    if (uVar5 != 0) {
      return 2;
    }
LAB_ram_0001db1e:
    if ((iVar4 != iVar1) || (uVar2 < uVar3)) goto LAB_ram_0001db08;
  }
  else {
    if ((uVar2 == 0xff) && (uVar5 != 0)) {
      return 2;
    }
    if (uVar3 != 0) {
LAB_ram_0001db1a:
      if ((uVar2 == 0) && (uVar5 == 0)) goto LAB_ram_0001db08;
      goto LAB_ram_0001db1e;
    }
    if ((uVar2 == 0) && (uVar5 == 0)) {
      if (uVar6 == 0) {
        return 0;
      }
      goto LAB_ram_0001db08;
    }
    if (uVar6 == 0) goto joined_r0x0001db38;
    if (iVar4 != iVar1) goto LAB_ram_0001db08;
    uVar3 = 0;
  }
  iVar1 = iVar4;
  if (uVar2 <= uVar3) {
    if (uVar5 < uVar6) {
LAB_ram_0001db08:
      if (iVar4 != 0) {
        return 0xffffffff;
      }
      return 1;
    }
    if (uVar5 <= uVar6) {
      return 0;
    }
  }
joined_r0x0001db38:
  if (iVar1 == 0) {
    return 0xffffffff;
  }
  return 1;
}

