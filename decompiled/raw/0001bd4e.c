/* Original address: 0x0001bd4e; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001bd4e(uint param_1,uint param_2,uint param_3,uint param_4)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  
  gp = &__global_pointer;
  uVar5 = param_2 >> 0x14 & 0x7ff;
  uVar3 = param_2 & 0xfffff;
  if (uVar5 == 0) {
    if (uVar3 == 0 && param_1 == 0) {
      uVar5 = 4;
      param_1 = 0;
      iVar4 = 0;
      iVar16 = 1;
      uVar3 = 0;
    }
    else {
      if (uVar3 == 0) {
        iVar16 = FUN_ram_0001e4f8(param_1);
        iVar7 = iVar16 + 0x15;
        iVar4 = iVar16 + 0x20;
        if (iVar7 < 0x1d) goto LAB_ram_0001beac;
        uVar3 = param_1 << (iVar16 - 8U & 0x1f);
        param_1 = 0;
      }
      else {
        iVar4 = FUN_ram_0001e4f8(uVar3);
        iVar7 = iVar4 + -0xb;
LAB_ram_0001beac:
        uVar3 = param_1 >> (0x1dU - iVar7 & 0x1f) | uVar3 << (iVar4 - 8U & 0x1f);
        param_1 = param_1 << (iVar4 - 8U & 0x1f);
      }
      iVar4 = -0x3f3 - iVar4;
      uVar5 = 0;
      iVar16 = 0;
    }
  }
  else if (uVar5 == 0x7ff) {
    if (uVar3 == 0 && param_1 == 0) {
      uVar5 = 8;
      param_1 = 0;
      iVar4 = 0x7ff;
      iVar16 = 2;
      uVar3 = 0;
    }
    else {
      uVar5 = 0xc;
      iVar4 = 0x7ff;
      iVar16 = 3;
    }
  }
  else {
    uVar3 = param_1 >> 0x1d | uVar3 << 3 | 0x800000;
    param_1 = param_1 << 3;
    iVar4 = uVar5 - 0x3ff;
    uVar5 = 0;
    iVar16 = 0;
  }
  uVar8 = param_4 >> 0x14 & 0x7ff;
  uVar14 = param_4 & 0xfffff;
  if (uVar8 == 0) {
    if (uVar14 == 0 && param_3 == 0) {
      uVar5 = uVar5 | 1;
      param_3 = 0;
      iVar7 = 1;
      uVar14 = 0;
    }
    else {
      if (uVar14 == 0) {
        iVar6 = FUN_ram_0001e4f8(param_3);
        iVar9 = iVar6 + 0x15;
        iVar7 = iVar6 + 0x20;
        if (iVar9 < 0x1d) goto LAB_ram_0001be6a;
        uVar14 = param_3 << (iVar6 - 8U & 0x1f);
        param_3 = 0;
      }
      else {
        iVar7 = FUN_ram_0001e4f8(uVar14);
        iVar9 = iVar7 + -0xb;
LAB_ram_0001be6a:
        uVar14 = param_3 >> (0x1dU - iVar9 & 0x1f) | uVar14 << (iVar7 - 8U & 0x1f);
        param_3 = param_3 << (iVar7 - 8U & 0x1f);
      }
      iVar4 = iVar7 + iVar4 + 0x3f3;
      iVar7 = 0;
    }
  }
  else if (uVar8 == 0x7ff) {
    iVar4 = iVar4 + -0x7ff;
    if (uVar14 == 0 && param_3 == 0) {
      uVar5 = uVar5 | 2;
      param_3 = 0;
      iVar7 = 2;
      uVar14 = 0;
    }
    else {
      uVar5 = uVar5 | 3;
      iVar7 = 3;
    }
  }
  else {
    uVar14 = param_3 >> 0x1d | uVar14 << 3 | 0x800000;
    param_3 = param_3 << 3;
    iVar4 = iVar4 - (uVar8 - 0x3ff);
    iVar7 = 0;
  }
  uVar8 = (param_2 ^ param_4) >> 0x1f;
  switch(uVar5) {
  default:
    if ((uVar14 < uVar3) || ((uVar3 == uVar14 && (param_3 <= param_1)))) {
      uVar5 = uVar3 << 0x1f;
      uVar3 = uVar3 >> 1;
      uVar5 = uVar5 | param_1 >> 1;
      uVar15 = param_1 << 0x1f;
    }
    else {
      iVar4 = iVar4 + -1;
      uVar15 = 0;
      uVar5 = param_1;
    }
    uVar2 = param_3 >> 0x18 | uVar14 << 8;
    uVar18 = (uVar14 & 0xffffff) >> 8;
    uVar12 = uVar3 / uVar18;
    uVar19 = param_3 >> 0x18 | uVar14 << 8 & 0xffff;
    uVar17 = param_3 * 0x100;
    uVar10 = uVar19 * uVar12;
    uVar14 = uVar5 >> 0x10 | uVar3 % uVar18 << 0x10;
    uVar3 = uVar12;
    if (uVar14 < uVar10) {
      uVar14 = uVar14 + uVar2;
      uVar3 = uVar12 - 1;
      if ((uVar2 <= uVar14) && (uVar14 < uVar10)) {
        uVar3 = uVar12 - 2;
        uVar14 = uVar14 + uVar2;
      }
    }
    uVar11 = (uVar14 - uVar10) / uVar18;
    uVar12 = uVar19 * uVar11;
    uVar10 = (uVar14 - uVar10) % uVar18 << 0x10 | uVar5 & 0xffff;
    uVar5 = uVar11;
    if (uVar10 < uVar12) {
      uVar10 = uVar10 + uVar2;
      uVar5 = uVar11 - 1;
      if ((uVar2 <= uVar10) && (uVar10 < uVar12)) {
        uVar5 = uVar11 - 2;
        uVar10 = uVar10 + uVar2;
      }
    }
    uVar11 = uVar5 | uVar3 << 0x10;
    uVar20 = (param_3 & 0xffffff) >> 8;
    uVar13 = uVar17 & 0xffff;
    uVar3 = (uVar5 & 0xffff) * uVar13;
    uVar10 = uVar10 - uVar12;
    uVar14 = (uVar11 >> 0x10) * uVar13;
    uVar5 = uVar20 * (uVar5 & 0xffff) + uVar14 + (uVar3 >> 0x10);
    iVar16 = (uVar11 >> 0x10) * uVar20;
    if (uVar5 < uVar14) {
      iVar16 = iVar16 + 0x10000;
    }
    uVar12 = (uVar5 >> 0x10) + iVar16;
    uVar5 = uVar5 * 0x10000 + (uVar3 & 0xffff);
    if ((uVar10 < uVar12) || ((uVar14 = uVar11, uVar10 == uVar12 && (uVar15 < uVar5)))) {
      uVar15 = uVar15 + uVar17;
      uVar10 = uVar10 + (uVar15 < uVar17) + uVar2;
      uVar14 = uVar11 - 1;
      if (((uVar2 < uVar10) || ((uVar2 == uVar10 && (uVar17 <= uVar15)))) &&
         ((uVar10 < uVar12 || ((uVar12 == uVar10 && (uVar15 < uVar5)))))) {
        uVar15 = uVar15 + uVar17;
        uVar14 = uVar11 - 2;
        uVar10 = uVar10 + (uVar15 < uVar17) + uVar2;
      }
    }
    uVar5 = uVar15 - uVar5;
    uVar15 = (uVar10 - uVar12) - (uint)(uVar15 < uVar5);
    uVar3 = iVar4 + 0x3ff;
    if (uVar2 != uVar15) {
      uVar12 = uVar15 / uVar18;
      uVar10 = uVar19 * uVar12;
      uVar11 = uVar5 >> 0x10 | uVar15 % uVar18 << 0x10;
      uVar15 = uVar12;
      if (uVar11 < uVar10) {
        uVar11 = uVar11 + uVar2;
        uVar15 = uVar12 - 1;
        if ((uVar2 <= uVar11) && (uVar11 < uVar10)) {
          uVar15 = uVar12 - 2;
          uVar11 = uVar11 + uVar2;
        }
      }
      uVar12 = (uVar11 - uVar10) / uVar18;
      uVar19 = uVar19 * uVar12;
      uVar10 = uVar5 & 0xffff | (uVar11 - uVar10) % uVar18 << 0x10;
      uVar5 = uVar12;
      if (uVar10 < uVar19) {
        uVar10 = uVar10 + uVar2;
        uVar5 = uVar12 - 1;
        if ((uVar2 <= uVar10) && (uVar10 < uVar19)) {
          uVar5 = uVar12 - 2;
          uVar10 = uVar10 + uVar2;
        }
      }
      param_1 = uVar15 << 0x10 | uVar5;
      uVar15 = (uVar5 & 0xffff) * uVar13;
      uVar10 = uVar10 - uVar19;
      uVar13 = (param_1 >> 0x10) * uVar13;
      uVar5 = (uVar15 >> 0x10) + uVar20 * (uVar5 & 0xffff) + uVar13;
      iVar16 = uVar20 * (param_1 >> 0x10);
      if (uVar5 < uVar13) {
        iVar16 = iVar16 + 0x10000;
      }
      uVar12 = iVar16 + (uVar5 >> 0x10);
      uVar5 = uVar5 * 0x10000 + (uVar15 & 0xffff);
      if (uVar10 < uVar12) {
LAB_ram_0001c12a:
        uVar10 = uVar10 + uVar2;
        uVar15 = param_1 - 1;
        if (uVar10 < uVar2) {
joined_r0x0001c2e4:
          if (uVar10 != uVar12) goto LAB_ram_0001c13e;
        }
        else {
          if (uVar10 < uVar12) {
LAB_ram_0001c2c4:
            uVar10 = uVar10 + uVar2 + (param_3 << 9 < uVar17);
            uVar15 = param_1 - 2;
            uVar17 = param_3 << 9;
            goto joined_r0x0001c2e4;
          }
          if (uVar10 != uVar12) goto LAB_ram_0001c13e;
          if (uVar17 < uVar5) goto LAB_ram_0001c2c4;
        }
        param_1 = uVar15;
        if (uVar17 != uVar5) {
          param_1 = param_1 | 1;
        }
      }
      else {
        uVar15 = param_1;
        if (uVar10 == uVar12) {
          if (uVar5 != 0) goto LAB_ram_0001c12a;
        }
        else {
LAB_ram_0001c13e:
          param_1 = uVar15 | 1;
        }
      }
      goto LAB_ram_0001bf46;
    }
    param_1 = 0;
    uVar5 = 1;
    if ((int)uVar3 < 1) {
      param_1 = 0xffffffff;
      uVar5 = 0xffffffff;
      if (uVar3 == 0) {
        iVar4 = -0x3ff;
        goto LAB_ram_0001c208;
      }
      goto LAB_ram_0001bf18;
    }
    goto LAB_ram_0001c1f2;
  case 1:
  case 8:
  case 9:
    goto switchD_ram_0001bdfc_caseD_1;
  case 2:
  case 4:
  case 6:
    goto switchD_ram_0001bdfc_caseD_2;
  case 3:
  case 7:
  case 0xb:
    param_1 = param_3;
    break;
  case 5:
  case 10:
  case 0xf:
    goto switchD_ram_0001bdfc_caseD_5;
  case 0xc:
  case 0xd:
  case 0xe:
    iVar7 = iVar16;
    uVar14 = uVar3;
    param_4 = param_2;
  }
  uVar8 = param_4 >> 0x1f;
  if (iVar7 == 2) {
    uVar3 = 0x7ff;
    uVar5 = 0;
    param_1 = 0;
    goto LAB_ram_0001be1c;
  }
  if (iVar7 == 3) {
switchD_ram_0001bdfc_caseD_5:
    uVar8 = 0;
    uVar3 = 0x7ff;
    uVar5 = 0x80000;
    param_1 = 0;
    goto LAB_ram_0001be1c;
  }
  if (iVar7 == 1) {
switchD_ram_0001bdfc_caseD_2:
    uVar3 = 0;
    uVar5 = 0;
    param_1 = 0;
    goto LAB_ram_0001be1c;
  }
  uVar3 = iVar4 + 0x3ff;
LAB_ram_0001bf46:
  if (0 < (int)uVar3) {
    if (((param_1 & 7) == 0) || ((param_1 & 0xf) == 4)) {
      param_1 = param_1 >> 3;
    }
    else {
      uVar5 = param_1 < 0xfffffffc ^ 1;
      param_1 = param_1 + 4 >> 3;
LAB_ram_0001c1f2:
      uVar14 = uVar14 + uVar5;
    }
    if ((int)(uVar14 << 7) < 0) {
      uVar14 = uVar14 & 0xfeffffff;
      uVar3 = iVar4 + 0x400;
    }
    if ((int)uVar3 < 0x7ff) {
      uVar5 = (uVar14 & 0x7fffff) >> 3;
      param_1 = uVar14 << 0x1d | param_1;
      uVar3 = uVar3 & 0x7ff;
    }
    else {
switchD_ram_0001bdfc_caseD_1:
      uVar3 = 0x7ff;
      uVar5 = 0;
      param_1 = 0;
    }
    goto LAB_ram_0001be1c;
  }
  uVar5 = param_1;
  if (uVar3 == 0) {
LAB_ram_0001c208:
    uVar15 = 1;
LAB_ram_0001c20a:
    uVar2 = uVar14 << (iVar4 + 0x41eU & 0x1f) | uVar5 >> (uVar15 & 0x1f);
    uVar17 = (uint)(uVar5 << (iVar4 + 0x41eU & 0x1f) != 0);
    uVar3 = uVar2 | uVar17;
    uVar14 = uVar14 >> (uVar15 & 0x1f);
    uVar5 = uVar14;
    if (((uVar2 & 7) != 0 || uVar17 != 0) && ((uVar2 & 0xf | uVar17) != 4)) {
LAB_ram_0001c236:
      bVar1 = uVar3 + 4 < uVar3;
      uVar3 = uVar3 + 4;
      uVar5 = uVar14 + bVar1;
    }
    if ((int)(uVar5 << 8) < 0) {
      uVar3 = 1;
      uVar5 = 0;
      param_1 = 0;
      goto LAB_ram_0001be1c;
    }
LAB_ram_0001c296:
    param_1 = uVar5 << 0x1d;
    uVar5 = (uVar5 & 0x7fffff) >> 3;
  }
  else {
LAB_ram_0001bf18:
    uVar15 = 1 - uVar3;
    if (0x38 < (int)uVar15) goto switchD_ram_0001bdfc_caseD_2;
    uVar5 = param_1;
    if ((int)uVar15 < 0x20) goto LAB_ram_0001c20a;
    uVar17 = uVar14 >> (-uVar3 - 0x1f & 0x1f);
    if (uVar15 != 0x20) {
      param_1 = param_1 | uVar14 << (iVar4 + 0x43eU & 0x1f);
    }
    uVar15 = (uint)(param_1 != 0);
    uVar3 = uVar15 | uVar17;
    uVar5 = 0;
    param_1 = 0;
    if (uVar15 != 0 || (uVar17 & 7) != 0) {
      uVar14 = 0;
      uVar5 = 0;
      if ((uVar15 | uVar17 & 0xf) != 4) goto LAB_ram_0001c236;
      goto LAB_ram_0001c296;
    }
  }
  param_1 = uVar3 >> 3 | param_1;
  uVar3 = 0;
LAB_ram_0001be1c:
  return CONCAT44(uVar5 | uVar3 << 0x14 | uVar8 << 0x1f,param_1);
}

