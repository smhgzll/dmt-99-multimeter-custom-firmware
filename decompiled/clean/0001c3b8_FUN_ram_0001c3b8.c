/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001c3b8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001c3b8; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001c3b8(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  
  gp = &__global_pointer;
  uVar8 = param_2 >> 0x14 & 0x7ff;
  uVar10 = param_2 & 0xfffff;
  if (uVar8 == 0) {
    if (uVar10 == 0 && param_1 == 0) {
      uVar8 = 4;
      param_1 = 0;
      iVar9 = 0;
      iVar13 = 1;
      uVar10 = 0;
      goto LAB_ram_0001c40c;
    }
    if (uVar10 == 0) {
      iVar13 = FUN_ram_0001e4f8();
      iVar6 = iVar13 + 0x15;
      iVar9 = iVar13 + 0x20;
      if (iVar6 < 0x1d) goto LAB_ram_0001c4fa;
      uVar10 = param_1 << (iVar13 - 8U & 0x1f);
      param_1 = 0;
    }
    else {
      iVar9 = FUN_ram_0001e4f8(uVar10);
      iVar6 = iVar9 + -0xb;
LAB_ram_0001c4fa:
      uVar10 = param_1 >> (0x1dU - iVar6 & 0x1f) | uVar10 << (iVar9 - 8U & 0x1f);
      param_1 = param_1 << (iVar9 - 8U & 0x1f);
    }
    uVar5 = param_4 >> 0x14 & 0x7ff;
    iVar9 = -0x3f3 - iVar9;
    uVar8 = 0;
    iVar6 = 0;
    iVar13 = 0;
    uVar12 = 0;
    if (uVar5 != 0) goto LAB_ram_0001c426;
LAB_ram_0001c538:
    uVar12 = param_4 & 0xfffff;
    if (uVar12 == 0 && param_3 == 0) {
      uVar8 = uVar8 | 1;
      param_3 = 0;
      iVar6 = 1;
      uVar12 = 0;
    }
    else {
      if (uVar12 == 0) {
        iVar3 = FUN_ram_0001e4f8(param_3);
        iVar4 = iVar3 + 0x15;
        iVar6 = iVar3 + 0x20;
        if (iVar4 < 0x1d) goto LAB_ram_0001c552;
        uVar12 = param_3 << (iVar3 - 8U & 0x1f);
        param_3 = 0;
      }
      else {
        iVar6 = FUN_ram_0001e4f8(uVar12);
        iVar4 = iVar6 + -0xb;
LAB_ram_0001c552:
        uVar12 = param_3 >> (0x1dU - iVar4 & 0x1f) | uVar12 << (iVar6 - 8U & 0x1f);
        param_3 = param_3 << (iVar6 - 8U & 0x1f);
      }
      iVar9 = (iVar9 - iVar6) + -0x3f3;
      iVar6 = 0;
    }
  }
  else {
    if (uVar8 == 0x7ff) {
      if (uVar10 == 0 && param_1 == 0) {
        uVar8 = 8;
        param_1 = 0;
        iVar9 = 0x7ff;
        iVar13 = 2;
        uVar10 = 0;
      }
      else {
        uVar8 = 0xc;
        iVar9 = 0x7ff;
        iVar13 = 3;
      }
    }
    else {
      uVar10 = param_1 >> 0x1d | uVar10 << 3 | 0x800000;
      param_1 = param_1 << 3;
      iVar9 = uVar8 - 0x3ff;
      uVar8 = 0;
      iVar13 = 0;
    }
LAB_ram_0001c40c:
    uVar5 = param_4 >> 0x14 & 0x7ff;
    uVar12 = uVar8;
    iVar6 = iVar13;
    if (uVar5 == 0) goto LAB_ram_0001c538;
LAB_ram_0001c426:
    uVar8 = uVar12;
    uVar12 = param_4 & 0xfffff;
    iVar13 = iVar6;
    if (uVar5 == 0x7ff) {
      iVar9 = iVar9 + 0x7ff;
      if (uVar12 == 0 && param_3 == 0) {
        uVar8 = uVar8 | 2;
        param_3 = 0;
        iVar6 = 2;
        uVar12 = 0;
      }
      else {
        uVar8 = uVar8 | 3;
        iVar6 = 3;
      }
    }
    else {
      uVar12 = param_3 >> 0x1d | uVar12 << 3 | 0x800000;
      param_3 = param_3 << 3;
      iVar9 = iVar9 + (uVar5 - 0x3ff);
      iVar6 = 0;
    }
  }
  uVar5 = (param_2 ^ param_4) >> 0x1f;
  switch(uVar8) {
  default:
    uVar8 = param_1 >> 0x10;
    uVar14 = param_3 >> 0x10;
    param_1 = param_1 & 0xffff;
    param_3 = param_3 & 0xffff;
    uVar7 = (param_3 * param_1 >> 0x10) + uVar14 * param_1 + uVar8 * param_3;
    iVar13 = uVar8 * uVar14;
    if (uVar7 < uVar8 * param_3) {
      iVar13 = iVar13 + 0x10000;
    }
    uVar2 = uVar12 & 0xffff;
    uVar12 = uVar12 >> 0x10;
    uVar1 = uVar12 * param_1 + uVar8 * uVar2 + (uVar2 * param_1 >> 0x10);
    iVar6 = uVar8 * uVar12;
    if (uVar1 < uVar8 * uVar2) {
      iVar6 = iVar6 + 0x10000;
    }
    uVar18 = uVar10 >> 0x10;
    uVar10 = uVar10 & 0xffff;
    uVar17 = (uVar1 >> 0x10) + iVar6;
    uVar8 = uVar1 * 0x10000 + (uVar2 * param_1 & 0xffff);
    uVar1 = (param_3 * uVar10 >> 0x10) + uVar14 * uVar10 + uVar18 * param_3;
    iVar6 = uVar14 * uVar18;
    if (uVar1 < uVar18 * param_3) {
      iVar6 = iVar6 + 0x10000;
    }
    uVar14 = uVar1 * 0x10000 + (param_3 * uVar10 & 0xffff);
    uVar15 = iVar6 + (uVar1 >> 0x10);
    uVar1 = uVar12 * uVar10 + uVar18 * uVar2 + (uVar2 * uVar10 >> 0x10);
    iVar6 = uVar12 * uVar18;
    if (uVar1 < uVar18 * uVar2) {
      iVar6 = iVar6 + 0x10000;
    }
    uVar18 = (uVar7 >> 0x10) + uVar8 + iVar13;
    uVar11 = (uint)(uVar18 < uVar8);
    uVar2 = uVar1 * 0x10000 + (uVar2 * uVar10 & 0xffff) + uVar17;
    uVar18 = uVar18 + uVar14;
    uVar10 = uVar2 + uVar11;
    uVar14 = (uint)(uVar18 < uVar14);
    uVar16 = uVar10 + uVar15;
    uVar12 = uVar16 + uVar14;
    uVar8 = (uint)((uVar18 & 0x7fffff) != 0 || (param_3 * param_1 & 0xffff) + uVar7 * 0x10000 != 0);
    uVar10 = ((uVar1 >> 0x10) + (uint)(uVar2 < uVar17 || uVar10 < uVar11) +
              (uint)(uVar16 < uVar15 || uVar12 < uVar14) + iVar6) * 0x200 | uVar12 >> 0x17;
    param_1 = uVar8 | uVar18 >> 0x17 | uVar12 * 0x200;
    if ((int)(uVar10 << 7) < 0) {
      param_1 = uVar8 | uVar18 >> 0x17 & 1 | param_1 >> 1 | (uVar12 >> 0x17) << 0x1f;
      uVar10 = uVar10 >> 1;
      iVar9 = iVar9 + 1;
    }
    goto LAB_ram_0001c728;
  case 3:
  case 7:
  case 0xb:
    uVar5 = param_4 >> 0x1f;
  case 1:
  case 2:
    uVar10 = uVar12;
    param_1 = param_3;
    iVar13 = iVar6;
  case 4:
  case 5:
  case 8:
  case 10:
    break;
  case 6:
  case 9:
  case 0xf:
    goto switchD_ram_0001c46e_caseD_6;
  case 0xc:
  case 0xd:
  case 0xe:
    uVar5 = param_2 >> 0x1f;
  }
  if (iVar13 == 2) {
    uVar8 = 0x7ff;
    uVar10 = 0;
    uVar12 = 0;
    goto LAB_ram_0001c492;
  }
  if (iVar13 == 3) {
switchD_ram_0001c46e_caseD_6:
    uVar5 = 0;
    uVar8 = 0x7ff;
    uVar10 = 0x80000;
    uVar12 = 0;
    goto LAB_ram_0001c492;
  }
  iVar9 = iVar9 + 1;
  if (iVar13 == 1) {
LAB_ram_0001c4d4:
    uVar8 = 0;
    uVar10 = 0;
    uVar12 = 0;
    goto LAB_ram_0001c492;
  }
LAB_ram_0001c728:
  uVar8 = iVar9 + 0x3ff;
  if (0 < (int)uVar8) {
    uVar12 = param_1;
    if (((param_1 & 7) != 0) && ((param_1 & 0xf) != 4)) {
      uVar12 = param_1 + 4;
      uVar10 = uVar10 + (uVar12 < param_1);
    }
    if ((int)(uVar10 << 7) < 0) {
      uVar10 = uVar10 & 0xfeffffff;
      uVar8 = iVar9 + 0x400;
    }
    if ((int)uVar8 < 0x7ff) {
      uVar12 = uVar10 << 0x1d | uVar12 >> 3;
      uVar10 = (uVar10 & 0x7fffff) >> 3;
      uVar8 = uVar8 & 0x7ff;
    }
    else {
      uVar8 = 0x7ff;
      uVar10 = 0;
      uVar12 = 0;
    }
    goto LAB_ram_0001c492;
  }
  uVar12 = 1 - uVar8;
  if (uVar8 == 0) {
LAB_ram_0001c7c4:
    uVar14 = (uint)(param_1 << (iVar9 + 0x41eU & 0x1f) != 0);
    uVar8 = uVar10 << (iVar9 + 0x41eU & 0x1f) | param_1 >> (uVar12 & 0x1f);
    uVar7 = uVar8 | uVar14;
    uVar10 = uVar10 >> (uVar12 & 0x1f);
    uVar12 = uVar10;
    if (((uVar8 & 7) != 0 || uVar14 != 0) && ((uVar8 & 0xf | uVar14) != 4)) {
LAB_ram_0001c7f2:
      uVar12 = uVar10 + (uVar7 + 4 < uVar7);
      uVar7 = uVar7 + 4;
    }
    if ((int)(uVar12 << 8) < 0) {
      uVar8 = 1;
      uVar10 = 0;
      uVar12 = 0;
      goto LAB_ram_0001c492;
    }
LAB_ram_0001c852:
    uVar10 = (uVar12 & 0x7fffff) >> 3;
    uVar12 = uVar12 << 0x1d;
  }
  else {
    if (0x38 < (int)uVar12) goto LAB_ram_0001c4d4;
    if ((int)uVar12 < 0x20) goto LAB_ram_0001c7c4;
    uVar8 = uVar10 >> (-uVar8 - 0x1f & 0x1f);
    if (uVar12 != 0x20) {
      param_1 = param_1 | uVar10 << (iVar9 + 0x43eU & 0x1f);
    }
    uVar14 = (uint)(param_1 != 0);
    uVar7 = uVar14 | uVar8;
    uVar10 = 0;
    uVar12 = 0;
    if (uVar14 != 0 || (uVar8 & 7) != 0) {
      uVar10 = 0;
      uVar12 = 0;
      if ((uVar14 | uVar8 & 0xf) != 4) goto LAB_ram_0001c7f2;
      goto LAB_ram_0001c852;
    }
  }
  uVar12 = uVar7 >> 3 | uVar12;
  uVar8 = 0;
LAB_ram_0001c492:
  return CONCAT44(uVar10 | uVar8 << 0x14 | uVar5 << 0x1f,uVar12);
}

