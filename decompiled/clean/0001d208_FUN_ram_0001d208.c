/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001d208; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001d208; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001d208(uint param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  
  gp = &__global_pointer;
  if (param_1 == 0 && param_2 == 0) {
    uVar7 = 0;
    uVar5 = 0;
    uVar6 = 0;
    param_1 = 0;
    goto LAB_ram_0001d25e;
  }
  uVar7 = param_2 >> 0x1f;
  if ((int)param_2 < 0) {
    param_1 = -param_1;
    param_2 = -(uint)(param_1 != 0) - param_2;
    if (param_2 != 0) goto LAB_ram_0001d22a;
LAB_ram_0001d292:
    iVar8 = FUN_ram_0001e4f8(param_1);
    iVar1 = iVar8 + 0x20;
    uVar5 = 0x43eU - iVar1 & 0x7ff;
    if (0x2a < iVar1) {
      uVar6 = param_1 << (iVar8 - 0xbU & 0x1f) & 0xfffff;
      param_1 = 0;
      goto LAB_ram_0001d25e;
    }
    uVar3 = iVar8 + 0x15;
  }
  else {
    if (param_2 == 0) goto LAB_ram_0001d292;
LAB_ram_0001d22a:
    iVar1 = FUN_ram_0001e4f8(param_2);
    uVar5 = 0x43e - iVar1;
    if (0x433 < (int)uVar5) {
      uVar6 = param_2;
      if (0x436 < (int)uVar5) {
        uVar3 = -iVar1 + 8;
        uVar6 = -iVar1 - 0x18;
        if ((int)uVar6 < 0) {
          uVar2 = param_1 >> (uVar3 & 0x1f) | (param_2 << 1) << (0x1f - uVar3 & 0x1f);
          uVar6 = param_2 >> (uVar3 & 0x1f);
        }
        else {
          uVar2 = param_2 >> (uVar6 & 0x1f);
          uVar6 = 0;
        }
        uVar3 = iVar1 + 0x38;
        if ((int)(iVar1 + 0x18U) < 0) {
          uVar4 = (param_1 >> 1) >> (0x1f - uVar3 & 0x1f) | param_2 << (uVar3 & 0x1f);
          iVar8 = param_1 << (uVar3 & 0x1f);
        }
        else {
          uVar4 = param_1 << (iVar1 + 0x18U & 0x1f);
          iVar8 = 0;
        }
        param_1 = uVar2 | (iVar8 != 0 || uVar4 != 0);
      }
      if (8 < iVar1) {
        uVar6 = param_1 >> (0x28U - iVar1 & 0x1f) | uVar6 << (iVar1 - 8U & 0x1f);
        param_1 = param_1 << (iVar1 - 8U & 0x1f);
      }
      uVar6 = uVar6 & 0xff7fffff;
      uVar3 = param_1;
      if (((param_1 & 7) != 0) && ((param_1 & 0xf) != 4)) {
        uVar3 = param_1 + 4;
        uVar6 = uVar6 + (uVar3 < param_1);
        if ((int)(uVar6 * 0x100) < 0) {
          uVar6 = uVar6 & 0xff7fffff;
          uVar5 = 0x43f - iVar1;
        }
      }
      param_1 = uVar6 << 0x1d | uVar3 >> 3;
      uVar6 = (uVar6 & 0x7fffff) >> 3;
      uVar5 = uVar5 & 0x7ff;
      goto LAB_ram_0001d25e;
    }
    uVar5 = uVar5 & 0x7ff;
    uVar6 = param_2 & 0xfffff;
    if (iVar1 < 0xc) goto LAB_ram_0001d25e;
    uVar3 = iVar1 - 0xb;
  }
  uVar6 = (param_1 >> (0x2bU - iVar1 & 0x1f) | param_2 << (uVar3 & 0x1f)) & 0xfffff;
  param_1 = param_1 << (uVar3 & 0x1f);
LAB_ram_0001d25e:
  return CONCAT44(uVar6 | uVar5 << 0x14 | uVar7 << 0x1f,param_1);
}

