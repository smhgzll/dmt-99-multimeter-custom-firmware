/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001d3be; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001d3be; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001d3be(uint param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  
  gp = &__global_pointer;
  uVar2 = param_1 & 0x7fffff;
  uVar3 = param_1 >> 0x17 & 0xff;
  uVar5 = param_2 & 0x7fffff;
  uVar8 = param_2 >> 0x17 & 0xff;
  param_1 = param_1 >> 0x1f;
  param_2 = param_2 >> 0x1f;
  uVar10 = uVar2 * 8;
  uVar11 = uVar5 * 8;
  uVar9 = uVar3 - uVar8;
  uVar7 = uVar2;
  uVar6 = uVar5;
  if (param_1 == param_2) {
    if ((int)uVar9 < 1) {
      if (uVar9 != 0) {
        uVar6 = uVar8 - uVar3;
        if (uVar3 == 0) {
          uVar7 = uVar5;
          param_2 = param_1;
          if (uVar2 == 0) goto LAB_ram_0001d5e8;
          uVar6 = uVar6 - 1;
          uVar7 = uVar10 + uVar11;
          if (uVar6 == 0) goto LAB_ram_0001d52a;
          if (uVar8 != 0xff) goto LAB_ram_0001d64a;
        }
        else if (uVar8 != 0xff) {
          uVar10 = uVar10 | 0x4000000;
LAB_ram_0001d64a:
          if ((int)uVar6 < 0x1c) {
            uVar7 = (uint)(uVar10 << (0x20 - uVar6 & 0x1f) != 0) | uVar10 >> (uVar6 & 0x1f);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar7 + uVar11;
          goto LAB_ram_0001d52a;
        }
        goto LAB_ram_0001d564;
      }
      uVar8 = uVar3 + 1;
      if ((uVar8 & 0xfe) == 0) {
        if (uVar3 == 0) {
          if ((uVar2 == 0) || (uVar6 = uVar2, uVar5 == 0)) goto LAB_ram_0001d4a6;
          uVar7 = uVar10 + uVar11;
          uVar8 = 0;
          param_2 = param_1;
          if ((int)(uVar7 * 0x20) < 0) {
            uVar7 = uVar7 & 0xfbffffff;
            uVar8 = 1;
          }
LAB_ram_0001d5dc:
          if ((uVar7 & 7) == 0) goto LAB_ram_0001d5e4;
          goto LAB_ram_0001d47e;
        }
        if (uVar2 != 0) goto LAB_ram_0001d6ca;
        goto LAB_ram_0001d564;
      }
      if (uVar8 != 0xff) {
        uVar7 = uVar10 + uVar11 >> 1;
        param_2 = param_1;
        goto LAB_ram_0001d5dc;
      }
    }
    else {
      if (uVar8 == 0) {
        uVar8 = uVar3;
        param_2 = param_1;
        if (uVar5 == 0) goto LAB_ram_0001d5e8;
        uVar9 = uVar9 - 1;
        uVar7 = uVar10 + uVar11;
        if (uVar9 == 0) goto LAB_ram_0001d52a;
        uVar5 = uVar2;
        if (uVar3 != 0xff) goto LAB_ram_0001d50a;
      }
      else {
        uVar5 = uVar2;
        if (uVar3 != 0xff) {
          uVar11 = uVar11 | 0x4000000;
LAB_ram_0001d50a:
          if ((int)uVar9 < 0x1c) {
            uVar7 = uVar11 >> (uVar9 & 0x1f) | (uint)(uVar11 << (0x20 - uVar9 & 0x1f) != 0);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar7 + uVar10;
          uVar8 = uVar3;
LAB_ram_0001d52a:
          param_2 = param_1;
          if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001d5dc;
          uVar8 = uVar8 + 1;
          if (uVar8 == 0xff) goto LAB_ram_0001d600;
          uVar7 = uVar7 & 1 | uVar7 >> 1 & 0x7dffffff;
LAB_ram_0001d478:
          param_2 = param_1;
          if ((uVar7 & 7) != 0) {
LAB_ram_0001d47e:
            param_1 = param_2;
            if ((uVar7 & 0xf) != 4) {
              uVar7 = uVar7 + 4;
            }
          }
          param_2 = param_1;
          if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001d5e4;
          if (uVar8 + 1 != 0xff) {
            uVar3 = uVar8 + 1 & 0xff;
            uVar6 = (uVar7 & 0x3ffffff) >> 3;
            goto LAB_ram_0001d4a6;
          }
          goto LAB_ram_0001d600;
        }
      }
LAB_ram_0001d564:
      if (uVar5 != 0) goto LAB_ram_0001d568;
    }
LAB_ram_0001d600:
    uVar3 = 0xff;
    uVar6 = 0;
  }
  else {
    if (0 < (int)uVar9) {
      if (uVar8 == 0) {
        uVar8 = uVar3;
        param_2 = param_1;
        if (uVar5 == 0) goto LAB_ram_0001d5e8;
        uVar9 = uVar9 - 1;
        uVar7 = uVar10 + uVar5 * -8;
        if (uVar9 == 0) goto LAB_ram_0001d43c;
        uVar5 = uVar2;
        if (uVar3 != 0xff) goto LAB_ram_0001d41a;
      }
      else {
        uVar5 = uVar2;
        if (uVar3 != 0xff) {
          uVar11 = uVar11 | 0x4000000;
LAB_ram_0001d41a:
          if ((int)uVar9 < 0x1c) {
            uVar7 = uVar11 >> (uVar9 & 0x1f) | (uint)(uVar11 << (0x20 - uVar9 & 0x1f) != 0);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar10 - uVar7;
          uVar8 = uVar3;
          param_2 = param_1;
LAB_ram_0001d43c:
          if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001d5dc;
          uVar7 = uVar7 & 0x3ffffff;
          uVar3 = uVar8;
LAB_ram_0001d44c:
          iVar4 = FUN_ram_0001e4f8(uVar7);
          uVar8 = iVar4 - 5;
          uVar5 = uVar7 << (uVar8 & 0x1f);
          if ((int)uVar3 <= (int)uVar8) {
            uVar3 = (uVar8 - uVar3) + 1;
            uVar7 = uVar5 >> (uVar3 & 0x1f) | (uint)(uVar5 << (0x20 - uVar3 & 0x1f) != 0);
            uVar8 = 0;
            param_1 = param_2;
            goto LAB_ram_0001d478;
          }
          uVar7 = uVar5 & 0xfbffffff;
          uVar8 = uVar3 - uVar8;
          if ((uVar5 & 7) != 0) goto LAB_ram_0001d47e;
LAB_ram_0001d5e4:
          uVar7 = uVar7 >> 3;
LAB_ram_0001d5e8:
          uVar5 = uVar7;
          param_1 = param_2;
          if (uVar8 != 0xff) {
            uVar3 = uVar8 & 0xff;
            uVar6 = uVar7 & 0x7fffff;
            goto LAB_ram_0001d4a6;
          }
        }
      }
      goto LAB_ram_0001d564;
    }
    if (uVar9 != 0) {
      uVar6 = uVar8 - uVar3;
      if (uVar3 == 0) {
        uVar7 = uVar5;
        if (uVar2 == 0) goto LAB_ram_0001d5e8;
        uVar6 = uVar6 - 1;
        if (uVar6 == 0) {
          uVar7 = uVar11 + uVar2 * -8;
          goto LAB_ram_0001d43c;
        }
        param_1 = param_2;
        if (uVar8 != 0xff) goto LAB_ram_0001d67a;
      }
      else {
        param_1 = param_2;
        if (uVar8 != 0xff) {
          uVar10 = uVar10 | 0x4000000;
LAB_ram_0001d67a:
          if ((int)uVar6 < 0x1c) {
            uVar7 = uVar10 >> (uVar6 & 0x1f) | (uint)(uVar10 << (0x20 - uVar6 & 0x1f) != 0);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar11 - uVar7;
          goto LAB_ram_0001d43c;
        }
      }
      goto LAB_ram_0001d564;
    }
    if ((uVar3 + 1 & 0xfe) != 0) {
      uVar7 = uVar10 + uVar5 * -8;
      if ((int)(uVar7 * 0x20) < 0) {
        uVar7 = uVar11 + uVar2 * -8;
      }
      else {
        param_2 = param_1;
        if (uVar7 == 0) {
          uVar3 = 0;
          param_1 = 0;
          uVar6 = 0;
          goto LAB_ram_0001d4a6;
        }
      }
      goto LAB_ram_0001d44c;
    }
    if (uVar3 == 0) {
      if (uVar2 == 0) {
        param_1 = param_2;
        if (uVar5 != 0) goto LAB_ram_0001d4a6;
      }
      else {
        uVar6 = uVar2;
        if (uVar5 == 0) goto LAB_ram_0001d4a6;
        uVar7 = uVar10 + uVar5 * -8;
        if ((int)(uVar7 * 0x20) < 0) {
          uVar7 = uVar11 + uVar2 * -8;
          uVar8 = uVar3;
          param_1 = param_2;
          goto LAB_ram_0001d478;
        }
        uVar8 = uVar3;
        param_2 = param_1;
        if (uVar7 != 0) goto LAB_ram_0001d5dc;
      }
      param_1 = 0;
      uVar6 = 0;
      goto LAB_ram_0001d4a6;
    }
    if (uVar2 == 0) {
      param_1 = param_2;
      if (uVar5 != 0) goto LAB_ram_0001d564;
    }
    else {
LAB_ram_0001d6ca:
      bVar1 = uVar5 == 0;
      uVar5 = uVar2;
      if (bVar1) goto LAB_ram_0001d564;
    }
LAB_ram_0001d568:
    uVar3 = 0xff;
    param_1 = 0;
    uVar6 = 0x400000;
  }
LAB_ram_0001d4a6:
  return uVar6 | uVar3 << 0x17 | param_1 << 0x1f;
}

