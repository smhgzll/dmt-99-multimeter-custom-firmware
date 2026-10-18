/* Original address: 0x0001db48; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001db48(uint param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  
  gp = &__global_pointer;
  uVar1 = param_1 >> 0x17 & 0xff;
  uVar6 = param_1 & 0x7fffff;
  if (uVar1 == 0) {
    if (uVar6 != 0) {
      iVar2 = FUN_ram_0001e4f8(uVar6);
      uVar6 = uVar6 << (iVar2 - 5U & 0x1f);
      iVar2 = -0x76 - iVar2;
      uVar1 = 0;
      iVar9 = 0;
      goto LAB_ram_0001db8c;
    }
    uVar4 = param_2 >> 0x17 & 0xff;
    uVar1 = 4;
    iVar3 = 0;
    iVar2 = 0;
    iVar10 = 1;
    iVar9 = 1;
    uVar7 = 4;
    if (uVar4 != 0) goto LAB_ram_0001dba2;
LAB_ram_0001dc58:
    uVar7 = param_2 & 0x7fffff;
    if (uVar7 == 0) {
      uVar1 = uVar1 | 1;
      iVar3 = 1;
      uVar7 = 0;
    }
    else {
      iVar3 = FUN_ram_0001e4f8(uVar7);
      uVar7 = uVar7 << (iVar3 - 5U & 0x1f);
      iVar2 = (iVar2 - iVar3) + -0x76;
      iVar3 = 0;
    }
  }
  else {
    if (uVar1 == 0xff) {
      if (uVar6 == 0) {
        uVar1 = 8;
        iVar2 = 0xff;
        iVar9 = 2;
      }
      else {
        uVar1 = 0xc;
        iVar2 = 0xff;
        iVar9 = 3;
      }
    }
    else {
      uVar6 = uVar6 << 3 | 0x4000000;
      iVar2 = uVar1 - 0x7f;
      uVar1 = 0;
      iVar9 = 0;
    }
LAB_ram_0001db8c:
    uVar4 = param_2 >> 0x17 & 0xff;
    iVar3 = iVar2;
    uVar7 = uVar1;
    iVar10 = iVar9;
    if (uVar4 == 0) goto LAB_ram_0001dc58;
LAB_ram_0001dba2:
    uVar1 = uVar7;
    uVar7 = param_2 & 0x7fffff;
    iVar9 = iVar10;
    if (uVar4 == 0xff) {
      iVar2 = iVar3 + 0xff;
      if (uVar7 == 0) {
        uVar1 = uVar1 | 2;
        iVar3 = 2;
      }
      else {
        uVar1 = uVar1 | 3;
        iVar3 = 3;
      }
    }
    else {
      uVar7 = uVar7 << 3 | 0x4000000;
      iVar2 = iVar3 + (uVar4 - 0x7f);
      iVar3 = 0;
    }
  }
  uVar4 = (param_1 ^ param_2) >> 0x1f;
  switch(uVar1) {
  default:
    uVar5 = (uVar6 & 0xffff) * (uVar7 & 0xffff);
    uVar8 = (uVar6 >> 0x10) * (uVar7 & 0xffff);
    uVar1 = (uVar5 >> 0x10) + (uVar7 >> 0x10) * (uVar6 & 0xffff) + uVar8;
    iVar9 = (uVar6 >> 0x10) * (uVar7 >> 0x10);
    if (uVar1 < uVar8) {
      iVar9 = iVar9 + 0x10000;
    }
    uVar7 = uVar1 * 0x10000 + (uVar5 & 0xffff);
    uVar5 = (uint)((uVar7 & 0x3ffffff) != 0);
    uVar7 = uVar7 >> 0x1a;
    uVar6 = ((uVar1 >> 0x10) + iVar9) * 0x40 | uVar5 | uVar7;
    if ((int)(uVar6 << 4) < 0) {
      uVar6 = uVar6 >> 1 | uVar5 | uVar7 & 1;
      iVar2 = iVar2 + 1;
    }
    goto LAB_ram_0001dd40;
  case 3:
  case 7:
  case 0xb:
    uVar4 = param_2 >> 0x1f;
  case 1:
  case 2:
    uVar6 = uVar7;
    iVar9 = iVar3;
  case 4:
  case 5:
  case 8:
  case 10:
    break;
  case 6:
  case 9:
  case 0xf:
    goto switchD_ram_0001dbdc_caseD_6;
  case 0xc:
  case 0xd:
  case 0xe:
    uVar4 = param_1 >> 0x1f;
  }
  if (iVar9 == 2) {
    uVar1 = 0xff;
    uVar7 = 0;
  }
  else if (iVar9 == 3) {
switchD_ram_0001dbdc_caseD_6:
    uVar4 = 0;
    uVar1 = 0xff;
    uVar7 = 0x400000;
  }
  else {
    iVar2 = iVar2 + 1;
    if (iVar9 == 1) {
      uVar1 = 0;
      uVar7 = 0;
    }
    else {
LAB_ram_0001dd40:
      uVar5 = iVar2 + 0x7f;
      if ((int)uVar5 < 1) {
        if (uVar5 != 0) {
          uVar1 = 0;
          uVar7 = 0;
          if (0x1b < (int)(1 - uVar5)) goto LAB_ram_0001dbf8;
        }
        uVar1 = (uint)(uVar6 << (iVar2 + 0x9eU & 0x1f) != 0);
        uVar6 = uVar6 >> (1 - uVar5 & 0x1f);
        uVar7 = uVar6 | uVar1;
        if (((uVar6 & 7) != 0 || uVar1 != 0) && ((uVar6 & 0xf | uVar1) != 4)) {
          uVar7 = uVar7 + 4;
        }
        if ((int)(uVar7 << 5) < 0) {
          uVar1 = 1;
          uVar7 = 0;
        }
        else {
          uVar7 = (uVar7 & 0x3ffffff) >> 3;
          uVar1 = 0;
        }
      }
      else {
        if (((uVar6 & 7) != 0) && ((uVar6 & 0xf) != 4)) {
          uVar6 = uVar6 + 4;
        }
        if ((int)(uVar6 << 4) < 0) {
          uVar6 = uVar6 & 0xf7ffffff;
          uVar5 = iVar2 + 0x80;
        }
        if ((int)uVar5 < 0xff) {
          uVar7 = (uVar6 & 0x3ffffff) >> 3;
          uVar1 = uVar5 & 0xff;
        }
        else {
          uVar1 = 0xff;
          uVar7 = 0;
        }
      }
    }
  }
LAB_ram_0001dbf8:
  return uVar4 << 0x1f | uVar7 | uVar1 << 0x17;
}

