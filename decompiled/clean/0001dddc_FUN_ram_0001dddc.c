/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001dddc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001dddc; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001dddc(uint param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  
  gp = &__global_pointer;
  uVar6 = param_1 & 0x7fffff;
  uVar5 = param_1 >> 0x17 & 0xff;
  param_1 = param_1 >> 0x1f;
  uVar4 = param_2 & 0x7fffff;
  uVar8 = param_2 >> 0x17 & 0xff;
  uVar11 = uVar6 * 8;
  param_2 = param_2 >> 0x1f;
  uVar2 = uVar4 * 8;
  uVar9 = uVar5 - uVar8;
  uVar7 = uVar4;
  if ((uVar8 == 0xff) && (uVar4 != 0)) {
    if (param_1 == param_2) {
LAB_ram_0001dfdc:
      if (uVar9 == 0) {
        uVar8 = uVar5 + 1;
        if ((uVar8 & 0xfe) == 0) {
          if (uVar5 != 0) {
            param_2 = param_1;
            if (uVar6 != 0) {
LAB_ram_0001e106:
              bVar1 = uVar4 != 0;
              param_2 = param_1;
              uVar4 = uVar6;
              if (bVar1) goto LAB_ram_0001dfba;
            }
            goto LAB_ram_0001dfb6;
          }
          if ((uVar6 == 0) || (uVar7 = uVar6, uVar4 == 0)) goto LAB_ram_0001ded8;
          uVar7 = uVar11 + uVar2;
          uVar8 = 0;
          param_2 = param_1;
          if ((int)(uVar7 * 0x20) < 0) {
            uVar7 = uVar7 & 0xfbffffff;
            uVar8 = 1;
          }
        }
        else {
          param_2 = param_1;
          if (uVar8 == 0xff) goto LAB_ram_0001e038;
          uVar7 = uVar11 + uVar2 >> 1;
        }
LAB_ram_0001e010:
        if ((uVar7 & 7) != 0) goto LAB_ram_0001deac;
LAB_ram_0001e018:
        uVar10 = uVar7 >> 3;
        goto LAB_ram_0001e01c;
      }
      uVar9 = uVar8 - uVar5;
      if (uVar5 == 0) {
        param_2 = param_1;
        uVar10 = uVar4;
        if (uVar6 == 0) goto LAB_ram_0001e01c;
        uVar9 = uVar9 - 1;
        uVar7 = uVar11 + uVar2;
        if (uVar9 == 0) goto LAB_ram_0001df74;
        if (uVar8 != 0xff) goto LAB_ram_0001e084;
      }
      else {
        param_2 = param_1;
        if (uVar8 != 0xff) {
          uVar11 = uVar11 | 0x4000000;
LAB_ram_0001e084:
          if ((int)uVar9 < 0x1c) {
            uVar7 = uVar11 >> (uVar9 & 0x1f) | (uint)(uVar11 << (0x20 - uVar9 & 0x1f) != 0);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar7 + uVar2;
LAB_ram_0001df74:
          param_2 = param_1;
          if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001e010;
          uVar8 = uVar8 + 1;
          if (uVar8 != 0xff) {
            uVar7 = uVar7 & 1 | uVar7 >> 1 & 0x7dffffff;
            goto LAB_ram_0001dea6;
          }
          goto LAB_ram_0001e038;
        }
      }
      goto LAB_ram_0001dfb6;
    }
    if ((int)uVar9 < 1) {
LAB_ram_0001def4:
      if (uVar9 != 0) {
        uVar7 = uVar8 - uVar5;
        if (uVar5 == 0) {
          uVar10 = uVar4;
          if (uVar6 == 0) goto LAB_ram_0001e01c;
          uVar7 = uVar7 - 1;
          if (uVar7 == 0) {
            uVar7 = uVar2 + uVar6 * -8;
            goto LAB_ram_0001de6a;
          }
          if (uVar8 != 0xff) goto LAB_ram_0001e0b6;
        }
        else if (uVar8 != 0xff) {
          uVar11 = uVar11 | 0x4000000;
LAB_ram_0001e0b6:
          if ((int)uVar7 < 0x1c) {
            uVar7 = (uint)(uVar11 << (0x20 - uVar7 & 0x1f) != 0) | uVar11 >> (uVar7 & 0x1f);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar2 - uVar7;
          goto LAB_ram_0001de6a;
        }
        goto LAB_ram_0001dfb6;
      }
      if ((uVar5 + 1 & 0xfe) != 0) {
        uVar7 = uVar11 + uVar4 * -8;
        if ((int)(uVar7 * 0x20) < 0) {
          uVar7 = uVar2 + uVar6 * -8;
        }
        else {
          param_2 = param_1;
          if (uVar7 == 0) {
            uVar5 = 0;
            uVar7 = 0;
            param_1 = 0;
            goto LAB_ram_0001ded8;
          }
        }
        goto LAB_ram_0001de7a;
      }
      if (uVar5 == 0) {
        if (uVar6 == 0) {
          param_1 = param_2;
          if (uVar4 != 0) goto LAB_ram_0001ded8;
LAB_ram_0001e146:
          uVar7 = 0;
          param_1 = 0;
          goto LAB_ram_0001ded8;
        }
        uVar7 = uVar6;
        if (uVar4 == 0) goto LAB_ram_0001ded8;
        uVar7 = uVar11 + uVar4 * -8;
        if (-1 < (int)(uVar7 * 0x20)) {
          uVar8 = 0;
          param_2 = param_1;
          if (uVar7 == 0) goto LAB_ram_0001e146;
          goto LAB_ram_0001e010;
        }
        uVar7 = uVar2 + uVar6 * -8;
        uVar8 = 0;
        param_1 = param_2;
        goto LAB_ram_0001dea6;
      }
      if (uVar6 != 0) goto LAB_ram_0001e106;
      if (uVar4 != 0) goto LAB_ram_0001dfb6;
      goto LAB_ram_0001dfba;
    }
LAB_ram_0001df30:
    param_2 = param_1;
    uVar4 = uVar6;
    if (uVar5 == 0xff) goto LAB_ram_0001dfb6;
    uVar2 = uVar2 | 0x4000000;
LAB_ram_0001de4a:
    if ((int)uVar9 < 0x1c) {
      uVar7 = uVar2 >> (uVar9 & 0x1f) | (uint)(uVar2 << (0x20 - uVar9 & 0x1f) != 0);
    }
    else {
      uVar7 = 1;
    }
    uVar7 = uVar11 - uVar7;
    param_2 = param_1;
    uVar8 = uVar5;
LAB_ram_0001de6a:
    if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001e010;
    uVar7 = uVar7 & 0x3ffffff;
    uVar5 = uVar8;
LAB_ram_0001de7a:
    iVar3 = FUN_ram_0001e4f8(uVar7);
    uVar8 = iVar3 - 5;
    uVar4 = uVar7 << (uVar8 & 0x1f);
    if ((int)uVar8 < (int)uVar5) {
      uVar7 = uVar4 & 0xfbffffff;
      uVar8 = uVar5 - uVar8;
      if ((uVar4 & 7) == 0) goto LAB_ram_0001e018;
LAB_ram_0001deac:
      if ((uVar7 & 0xf) != 4) {
        uVar7 = uVar7 + 4;
      }
    }
    else {
      uVar5 = (uVar8 - uVar5) + 1;
      uVar7 = uVar4 >> (uVar5 & 0x1f) | (uint)(uVar4 << (0x20 - uVar5 & 0x1f) != 0);
      uVar8 = 0;
      param_1 = param_2;
LAB_ram_0001dea6:
      param_2 = param_1;
      if ((uVar7 & 7) != 0) goto LAB_ram_0001deac;
    }
    if (-1 < (int)(uVar7 << 5)) goto LAB_ram_0001e018;
    if (uVar8 + 1 != 0xff) {
      uVar5 = uVar8 + 1 & 0xff;
      uVar7 = (uVar7 & 0x3ffffff) >> 3;
      param_1 = param_2;
      goto LAB_ram_0001ded8;
    }
  }
  else {
    param_2 = param_2 ^ 1;
    uVar10 = uVar6;
    if (param_1 == param_2) {
      if ((int)uVar9 < 1) goto LAB_ram_0001dfdc;
      if (uVar8 == 0) {
        param_2 = param_1;
        uVar8 = uVar5;
        if (uVar4 == 0) goto LAB_ram_0001e01c;
        uVar9 = uVar9 - 1;
        uVar7 = uVar11 + uVar2;
        if (uVar9 == 0) goto LAB_ram_0001df74;
        uVar4 = uVar6;
        if (uVar5 != 0xff) goto LAB_ram_0001df56;
      }
      else {
        param_2 = param_1;
        uVar4 = uVar6;
        if (uVar5 != 0xff) {
          uVar2 = uVar2 | 0x4000000;
LAB_ram_0001df56:
          if ((int)uVar9 < 0x1c) {
            uVar7 = uVar2 >> (uVar9 & 0x1f) | (uint)(uVar2 << (0x20 - uVar9 & 0x1f) != 0);
          }
          else {
            uVar7 = 1;
          }
          uVar7 = uVar7 + uVar11;
          uVar8 = uVar5;
          goto LAB_ram_0001df74;
        }
      }
      goto LAB_ram_0001dfb6;
    }
    if ((int)uVar9 < 1) goto LAB_ram_0001def4;
    if (uVar8 != 0) goto LAB_ram_0001df30;
    param_2 = param_1;
    uVar8 = uVar5;
    if (uVar4 != 0) {
      uVar9 = uVar9 - 1;
      uVar7 = uVar11 + uVar4 * -8;
      if (uVar9 != 0) {
        uVar4 = uVar6;
        if (uVar5 == 0xff) goto LAB_ram_0001dfb6;
        goto LAB_ram_0001de4a;
      }
      goto LAB_ram_0001de6a;
    }
LAB_ram_0001e01c:
    uVar4 = uVar10;
    if (uVar8 != 0xff) {
      uVar5 = uVar8 & 0xff;
      uVar7 = uVar10 & 0x7fffff;
      param_1 = param_2;
      goto LAB_ram_0001ded8;
    }
LAB_ram_0001dfb6:
    if (uVar4 != 0) {
LAB_ram_0001dfba:
      uVar5 = 0xff;
      uVar7 = 0x400000;
      param_1 = 0;
      goto LAB_ram_0001ded8;
    }
  }
LAB_ram_0001e038:
  uVar5 = 0xff;
  uVar7 = 0;
  param_1 = param_2;
LAB_ram_0001ded8:
  return uVar7 | uVar5 << 0x17 | param_1 << 0x1f;
}

