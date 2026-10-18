/* Original address: 0x0001b61e; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001b61e(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
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
  uint uVar12;
  
  gp = &__global_pointer;
  uVar4 = param_2 & 0xfffff;
  uVar8 = param_4 & 0xfffff;
  uVar5 = param_1 >> 0x1d;
  uVar7 = param_3 >> 0x1d;
  uVar2 = param_2 >> 0x14 & 0x7ff;
  uVar1 = param_4 >> 0x14 & 0x7ff;
  param_2 = param_2 >> 0x1f;
  param_4 = param_4 >> 0x1f;
  uVar6 = uVar5 | uVar4 << 3;
  uVar11 = param_1 * 8;
  uVar9 = uVar7 | uVar8 << 3;
  uVar12 = param_3 * 8;
  uVar10 = uVar2 - uVar1;
  if (param_2 == param_4) {
    if ((int)uVar10 < 1) {
      if (uVar10 != 0) {
        uVar10 = uVar1 - uVar2;
        if (uVar2 == 0) {
          if (uVar6 == 0 && uVar11 == 0) {
            uVar1 = uVar7 << 0x1d | param_3 & 0x1fffffff;
            uVar2 = uVar10;
            goto LAB_ram_0001b930;
          }
          uVar2 = uVar10 - 1;
          if (uVar2 == 0) {
            uVar8 = (uint)(uVar11 + uVar12 < uVar12) + uVar6 + uVar9;
            goto LAB_ram_0001bae4;
          }
          if (uVar10 == 0x7ff) goto LAB_ram_0001bc92;
        }
        else {
          uVar6 = uVar6 | 0x800000;
          uVar2 = uVar10;
          if (uVar1 == 0x7ff) {
            uVar1 = uVar7 << 0x1d | param_3 & 0x1fffffff;
            goto LAB_ram_0001b98e;
          }
        }
        if ((int)uVar2 < 0x39) {
          if ((int)uVar2 < 0x20) {
            uVar10 = uVar6 << (0x20 - uVar2 & 0x1f) | uVar11 >> (uVar2 & 0x1f) |
                     (uint)(uVar11 << (0x20 - uVar2 & 0x1f) != 0);
            uVar9 = uVar9 + (uVar6 >> (uVar2 & 0x1f));
          }
          else {
            if (uVar2 != 0x20) {
              uVar11 = uVar11 | uVar6 << (0x40 - uVar2 & 0x1f);
            }
            uVar10 = (uint)(uVar11 != 0) | uVar6 >> (uVar2 - 0x20 & 0x1f);
          }
        }
        else {
          uVar10 = (uint)(uVar6 != 0 || uVar11 != 0);
        }
        uVar10 = uVar10 + uVar12;
        uVar8 = (uVar10 < uVar12) + uVar9;
        goto LAB_ram_0001b956;
      }
      uVar1 = uVar2 + 1;
      if ((uVar1 & 0x7fe) == 0) {
        if (uVar2 != 0) {
          if (uVar6 == 0 && uVar11 == 0) {
LAB_ram_0001bc92:
            uVar1 = param_3 & 0x1fffffff | uVar7 << 0x1d;
            goto LAB_ram_0001b98e;
          }
          if (uVar9 != 0 || uVar12 != 0) goto LAB_ram_0001b994;
          goto LAB_ram_0001bbba;
        }
        if (uVar6 == 0 && uVar11 == 0) {
          uVar1 = uVar7 << 0x1d | param_3 & 0x1fffffff;
          goto LAB_ram_0001b938;
        }
        if (uVar9 == 0 && uVar12 == 0) goto LAB_ram_0001bc0e;
        uVar10 = uVar11 + uVar12;
        uVar8 = uVar6 + uVar9 + (uint)(uVar10 < uVar11);
        if ((int)(uVar8 * 0x100) < 0) {
          uVar8 = uVar8 & 0xff7fffff;
          uVar2 = 1;
        }
      }
      else {
        if (uVar1 == 0x7ff) goto LAB_ram_0001b96a;
        uVar8 = uVar6 + uVar9 + (uint)(uVar11 + uVar12 < uVar11);
        uVar10 = uVar8 * -0x80000000 | uVar11 + uVar12 >> 1;
        uVar8 = uVar8 >> 1;
        uVar2 = uVar1;
      }
joined_r0x0001b91c:
      if ((uVar10 & 7) != 0) {
LAB_ram_0001b73a:
        if ((uVar10 & 0xf) != 4) {
          uVar8 = uVar8 + (uVar10 + 4 < uVar10);
          uVar10 = uVar10 + 4;
        }
LAB_ram_0001b750:
        if ((int)(uVar8 << 8) < 0) {
          if (uVar2 + 1 != 0x7ff) {
            uVar1 = uVar8 << 0x1d | uVar10 >> 3;
            uVar8 = (uVar8 & 0x7fffff) >> 3;
            uVar2 = uVar2 + 1 & 0x7ff;
            goto LAB_ram_0001b782;
          }
          goto LAB_ram_0001b96a;
        }
      }
      uVar1 = uVar10 >> 3 | uVar8 << 0x1d;
      uVar8 = uVar8 >> 3;
LAB_ram_0001b930:
      if (uVar2 != 0x7ff) {
LAB_ram_0001b938:
        uVar8 = uVar8 & 0xfffff;
        uVar2 = uVar2 & 0x7ff;
        goto LAB_ram_0001b782;
      }
      goto LAB_ram_0001b98e;
    }
    if (uVar1 == 0) {
      if (uVar9 == 0 && uVar12 == 0) {
        uVar1 = param_1 & 0x1fffffff | uVar5 << 0x1d;
        uVar2 = uVar10;
        uVar8 = uVar4;
        goto LAB_ram_0001b930;
      }
      uVar8 = uVar10 - 1;
      if (uVar8 != 0) goto joined_r0x0001b8ca;
      uVar8 = (uint)(uVar11 + uVar12 < uVar11) + uVar6 + uVar9;
LAB_ram_0001bae4:
      uVar10 = uVar11 + uVar12;
      uVar2 = 1;
      if (-1 < (int)(uVar8 << 8)) goto joined_r0x0001b91c;
      uVar2 = 2;
LAB_ram_0001baf0:
      uVar10 = uVar8 << 0x1f | uVar10 >> 1 | uVar10 & 1;
      uVar8 = (uVar8 & 0xff7fffff) >> 1;
LAB_ram_0001b734:
      if ((uVar10 & 7) != 0) goto LAB_ram_0001b73a;
      goto LAB_ram_0001b750;
    }
    uVar9 = uVar9 | 0x800000;
    uVar8 = uVar10;
    uVar10 = uVar2;
joined_r0x0001b8ca:
    if (uVar10 == 0x7ff) {
LAB_ram_0001bbba:
      uVar1 = param_1 & 0x1fffffff | uVar5 << 0x1d;
      uVar8 = uVar4;
      goto LAB_ram_0001b98e;
    }
    if ((int)uVar8 < 0x39) {
      if ((int)uVar8 < 0x20) {
        uVar10 = uVar9 << (0x20 - uVar8 & 0x1f) | uVar12 >> (uVar8 & 0x1f) |
                 (uint)(uVar12 << (0x20 - uVar8 & 0x1f) != 0);
        uVar6 = uVar6 + (uVar9 >> (uVar8 & 0x1f));
      }
      else {
        if (uVar8 != 0x20) {
          uVar12 = uVar12 | uVar9 << (0x40 - uVar8 & 0x1f);
        }
        uVar10 = (uint)(uVar12 != 0) | uVar9 >> (uVar8 - 0x20 & 0x1f);
      }
    }
    else {
      uVar10 = (uint)(uVar9 != 0 || uVar12 != 0);
    }
    uVar10 = uVar10 + uVar11;
    uVar8 = (uVar10 < uVar11) + uVar6;
    uVar1 = uVar2;
LAB_ram_0001b956:
    uVar2 = uVar1;
    if (-1 < (int)(uVar8 << 8)) goto joined_r0x0001b91c;
    uVar2 = uVar1 + 1;
    if (uVar2 != 0x7ff) goto LAB_ram_0001baf0;
LAB_ram_0001b96a:
    uVar2 = 0x7ff;
    uVar8 = 0;
    uVar1 = 0;
  }
  else {
    if ((int)uVar10 < 1) {
      if (uVar10 != 0) {
        uVar10 = uVar1 - uVar2;
        if (uVar2 == 0) {
          if (uVar6 == 0 && uVar11 == 0) {
            uVar1 = param_3 & 0x1fffffff | uVar7 << 0x1d;
            uVar2 = uVar10;
            param_2 = param_4;
            goto LAB_ram_0001b930;
          }
          uVar2 = uVar10 - 1;
          if (uVar2 == 0) {
            uVar10 = uVar12 + param_1 * -8;
            uVar8 = (uVar9 - uVar6) - (uint)(uVar12 < uVar10);
            uVar1 = 1;
            goto LAB_ram_0001b6cc;
          }
        }
        else {
          uVar6 = uVar6 | 0x800000;
          uVar2 = uVar10;
          uVar10 = uVar1;
        }
        if (uVar10 != 0x7ff) {
          if ((int)uVar2 < 0x39) {
            if ((int)uVar2 < 0x20) {
              uVar10 = uVar6 << (0x20 - uVar2 & 0x1f) | uVar11 >> (uVar2 & 0x1f) |
                       (uint)(uVar11 << (0x20 - uVar2 & 0x1f) != 0);
              uVar9 = uVar9 - (uVar6 >> (uVar2 & 0x1f));
            }
            else {
              if (uVar2 != 0x20) {
                uVar11 = uVar11 | uVar6 << (0x40 - uVar2 & 0x1f);
              }
              uVar10 = (uint)(uVar11 != 0) | uVar6 >> (uVar2 - 0x20 & 0x1f);
            }
          }
          else {
            uVar10 = (uint)(uVar6 != 0 || uVar11 != 0);
          }
          uVar10 = uVar12 - uVar10;
          uVar8 = uVar9 - (uVar12 < uVar10);
          goto LAB_ram_0001b6cc;
        }
        uVar1 = param_3 & 0x1fffffff | uVar7 << 0x1d;
        param_2 = param_4;
        goto LAB_ram_0001b98e;
      }
      if ((uVar2 + 1 & 0x7fe) != 0) {
        uVar10 = uVar11 + param_3 * -8;
        uVar8 = (uVar6 - uVar9) - (uint)(uVar11 < uVar10);
        if ((int)(uVar8 * 0x100) < 0) {
          uVar10 = uVar12 + param_1 * -8;
          uVar8 = (uVar9 - uVar6) - (uint)(uVar12 < uVar10);
        }
        else {
          param_4 = param_2;
          if (uVar10 == 0 && uVar8 == 0) {
            uVar8 = 0;
            param_2 = 0;
            uVar2 = 0;
            uVar1 = 0;
            goto LAB_ram_0001b938;
          }
        }
LAB_ram_0001b6de:
        if (uVar8 == 0) {
          iVar3 = FUN_ram_0001e4f8(uVar10);
          uVar1 = iVar3 + 0x18;
          if ((int)uVar1 < 0x20) goto LAB_ram_0001b6ec;
          uVar8 = uVar10 << (iVar3 - 8U & 0x1f);
          uVar10 = 0;
        }
        else {
          iVar3 = FUN_ram_0001e4f8(uVar8);
          uVar1 = iVar3 - 8;
LAB_ram_0001b6ec:
          uVar8 = uVar10 >> (0x20 - uVar1 & 0x1f) | uVar8 << (uVar1 & 0x1f);
          uVar10 = uVar10 << (uVar1 & 0x1f);
        }
        param_2 = param_4;
        if ((int)uVar1 < (int)uVar2) {
          uVar8 = uVar8 & 0xff7fffff;
          uVar2 = uVar2 - uVar1;
        }
        else {
          uVar9 = (uVar1 - uVar2) + 1;
          if ((int)uVar9 < 0x20) {
            uVar10 = uVar8 << (0x20 - uVar9 & 0x1f) | uVar10 >> (uVar9 & 0x1f) |
                     (uint)(uVar10 << (0x20 - uVar9 & 0x1f) != 0);
            uVar8 = uVar8 >> (uVar9 & 0x1f);
            uVar2 = 0;
            goto LAB_ram_0001b734;
          }
          if (uVar9 != 0x20) {
            uVar10 = uVar10 | uVar8 << (0x40 - uVar9 & 0x1f);
          }
          uVar10 = (uint)(uVar10 != 0) | uVar8 >> ((uVar1 - uVar2) - 0x1f & 0x1f);
          uVar8 = 0;
          uVar2 = 0;
        }
        goto joined_r0x0001b91c;
      }
      uVar1 = uVar9 | uVar12;
      if (uVar2 == 0) {
        if (uVar6 == 0 && uVar11 == 0) {
          if (uVar1 == 0) {
LAB_ram_0001bcae:
            uVar8 = 0;
            param_2 = 0;
          }
          else {
            uVar1 = uVar7 << 0x1d | param_3 & 0x1fffffff;
            param_2 = param_4;
          }
        }
        else {
          if (uVar1 != 0) {
            uVar10 = uVar11 + param_3 * -8;
            uVar8 = (uVar6 - uVar9) - (uint)(uVar11 < uVar10);
            if (-1 < (int)(uVar8 * 0x100)) {
              uVar1 = 0;
              if (uVar10 != 0 || uVar8 != 0) goto joined_r0x0001b91c;
              goto LAB_ram_0001bcae;
            }
            uVar10 = uVar12 + param_1 * -8;
            uVar8 = (uVar9 - uVar6) - (uint)(uVar12 < uVar10);
            param_2 = param_4;
            goto LAB_ram_0001b734;
          }
LAB_ram_0001bc0e:
          uVar1 = uVar5 << 0x1d | param_1 & 0x1fffffff;
          uVar8 = uVar4;
        }
        goto LAB_ram_0001b938;
      }
      if (uVar6 == 0 && uVar11 == 0) {
        if (uVar1 == 0) {
          param_2 = 0;
          uVar2 = 0x7ff;
          uVar8 = 0x80000;
          goto LAB_ram_0001b782;
        }
        uVar1 = uVar7 << 0x1d | param_3 & 0x1fffffff;
        param_2 = param_4;
        goto LAB_ram_0001b98e;
      }
      if (uVar1 == 0) goto LAB_ram_0001ba08;
    }
    else {
      if (uVar1 == 0) {
        if (uVar9 != 0 || uVar12 != 0) {
          uVar8 = uVar10 - 1;
          if (uVar8 == 0) {
            uVar10 = uVar11 + param_3 * -8;
            uVar8 = (uVar6 - uVar9) - (uint)(uVar11 < uVar10);
            uVar1 = 1;
            param_4 = param_2;
            goto LAB_ram_0001b6cc;
          }
          goto joined_r0x0001b856;
        }
        uVar1 = uVar5 << 0x1d | param_1 & 0x1fffffff;
        uVar2 = uVar10;
        uVar8 = uVar4;
        goto LAB_ram_0001b930;
      }
      uVar9 = uVar9 | 0x800000;
      uVar8 = uVar10;
      uVar10 = uVar2;
joined_r0x0001b856:
      if (uVar10 != 0x7ff) {
        if ((int)uVar8 < 0x39) {
          if ((int)uVar8 < 0x20) {
            uVar10 = uVar9 << (0x20 - uVar8 & 0x1f) | uVar12 >> (uVar8 & 0x1f) |
                     (uint)(uVar12 << (0x20 - uVar8 & 0x1f) != 0);
            uVar6 = uVar6 - (uVar9 >> (uVar8 & 0x1f));
          }
          else {
            if (uVar8 != 0x20) {
              uVar12 = uVar12 | uVar9 << (0x40 - uVar8 & 0x1f);
            }
            uVar10 = (uint)(uVar12 != 0) | uVar9 >> (uVar8 - 0x20 & 0x1f);
          }
        }
        else {
          uVar10 = (uint)(uVar9 != 0 || uVar12 != 0);
        }
        uVar10 = uVar11 - uVar10;
        uVar8 = uVar6 - (uVar11 < uVar10);
        uVar1 = uVar2;
        param_4 = param_2;
LAB_ram_0001b6cc:
        uVar2 = uVar1;
        param_2 = param_4;
        if ((int)(uVar8 << 8) < 0) {
          uVar8 = uVar8 & 0x7fffff;
          goto LAB_ram_0001b6de;
        }
        goto joined_r0x0001b91c;
      }
LAB_ram_0001ba08:
      uVar1 = uVar5 << 0x1d | param_1 & 0x1fffffff;
      uVar8 = uVar4;
LAB_ram_0001b98e:
      if (uVar1 == 0 && uVar8 == 0) goto LAB_ram_0001b96a;
    }
LAB_ram_0001b994:
    param_2 = 0;
    uVar2 = 0x7ff;
    uVar8 = 0x80000;
    uVar1 = 0;
  }
LAB_ram_0001b782:
  return CONCAT44(uVar8 | uVar2 << 0x14 | param_2 << 0x1f,uVar1);
}

