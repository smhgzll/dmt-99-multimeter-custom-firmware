/* Original address: 0x0001c870; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001c870(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
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
  uint uVar12;
  
  gp = &__global_pointer;
  uVar1 = param_2 & 0xfffff;
  uVar6 = param_4 & 0xfffff;
  uVar10 = param_3 >> 0x1d;
  uVar8 = param_1 >> 0x1d;
  uVar9 = param_4 >> 0x14 & 0x7ff;
  uVar7 = uVar10 | uVar6 << 3;
  uVar3 = param_2 >> 0x14 & 0x7ff;
  param_2 = param_2 >> 0x1f;
  uVar2 = uVar8 | uVar1 << 3;
  uVar12 = param_1 * 8;
  param_4 = param_4 >> 0x1f;
  uVar11 = param_3 * 8;
  if (uVar9 == 0x7ff) {
    uVar5 = uVar3 - 0x7ff;
    if (uVar7 == 0 && uVar11 == 0) {
      param_4 = param_4 ^ 1;
    }
    if (param_2 != param_4) {
LAB_ram_0001ca50:
      if (uVar5 == 0) {
        uVar5 = 0;
        if ((uVar3 + 1 & 0x7fe) != 0) {
          uVar1 = uVar12 + param_3 * -8;
          uVar6 = (uVar2 - uVar7) - (uint)(uVar12 < uVar1);
          if ((int)(uVar6 * 0x100) < 0) {
            uVar1 = uVar11 + param_1 * -8;
            uVar6 = (uVar7 - uVar2) - (uint)(uVar11 < uVar1);
          }
          else {
            uVar9 = 0;
            param_4 = param_2;
            if (uVar1 == 0 && uVar6 == 0) goto LAB_ram_0001cc6c;
          }
LAB_ram_0001c932:
          if (uVar6 == 0) {
            iVar4 = FUN_ram_0001e4f8(uVar1);
            uVar9 = iVar4 + 0x18;
            if ((int)uVar9 < 0x20) goto LAB_ram_0001c940;
            uVar6 = uVar1 << (iVar4 - 8U & 0x1f);
            uVar1 = 0;
          }
          else {
            iVar4 = FUN_ram_0001e4f8(uVar6);
            uVar9 = iVar4 - 8;
LAB_ram_0001c940:
            uVar6 = uVar1 >> (0x20 - uVar9 & 0x1f) | uVar6 << (uVar9 & 0x1f);
            uVar1 = uVar1 << (uVar9 & 0x1f);
          }
          param_2 = param_4;
          if ((int)uVar9 < (int)uVar3) {
            uVar6 = uVar6 & 0xff7fffff;
            uVar3 = uVar3 - uVar9;
          }
          else {
            uVar2 = (uVar9 - uVar3) + 1;
            if ((int)uVar2 < 0x20) {
              uVar1 = uVar6 << (0x20 - uVar2 & 0x1f) | uVar1 >> (uVar2 & 0x1f) |
                      (uint)(uVar1 << (0x20 - uVar2 & 0x1f) != 0);
              uVar6 = uVar6 >> (uVar2 & 0x1f);
              uVar3 = 0;
              goto LAB_ram_0001c986;
            }
            if (uVar2 != 0x20) {
              uVar1 = uVar1 | uVar6 << (0x40 - uVar2 & 0x1f);
            }
            uVar1 = (uint)(uVar1 != 0) | uVar6 >> ((uVar9 - uVar3) - 0x1f & 0x1f);
            uVar6 = 0;
            uVar3 = 0;
          }
          goto joined_r0x0001cc0a;
        }
        uVar9 = uVar7 | uVar11;
        if (uVar3 != 0) {
          if (uVar2 != 0 || uVar12 != 0) {
            if (uVar9 == 0) goto LAB_ram_0001ccd0;
            goto LAB_ram_0001cbee;
          }
          if (uVar9 == 0) {
            param_2 = 0;
            uVar5 = 0x7ff;
            uVar6 = 0x80000;
            goto LAB_ram_0001c9d4;
          }
          uVar9 = param_3 & 0x1fffffff | uVar10 << 0x1d;
          param_2 = param_4;
          goto LAB_ram_0001cbe8;
        }
        if (uVar2 == 0 && uVar12 == 0) {
          if (uVar9 == 0) goto LAB_ram_0001cc6c;
          uVar9 = param_3 & 0x1fffffff | uVar10 << 0x1d;
          param_2 = param_4;
        }
        else if (uVar9 == 0) {
          uVar9 = uVar8 << 0x1d | param_1 & 0x1fffffff;
          uVar6 = uVar1;
        }
        else {
          uVar1 = uVar12 + param_3 * -8;
          uVar6 = (uVar2 - uVar7) - (uint)(uVar12 < uVar1);
          if ((int)(uVar6 * 0x100) < 0) {
            uVar1 = uVar11 + param_1 * -8;
            uVar6 = (uVar7 - uVar2) - (uint)(uVar11 < uVar1);
            param_2 = param_4;
            goto LAB_ram_0001c986;
          }
          uVar9 = 0;
          if (uVar1 != 0 || uVar6 != 0) goto joined_r0x0001cc0a;
LAB_ram_0001cc6c:
          param_2 = 0;
          uVar6 = 0;
        }
        goto LAB_ram_0001cb8c;
      }
      uVar5 = uVar9 - uVar3;
      if (uVar3 == 0) {
        if (uVar2 == 0 && uVar12 == 0) {
          uVar9 = uVar10 << 0x1d | param_3 & 0x1fffffff;
          param_2 = param_4;
          goto LAB_ram_0001cb84;
        }
        uVar3 = uVar5 - 1;
        if (uVar3 == 0) {
          uVar1 = uVar11 + param_1 * -8;
          uVar6 = (uVar7 - uVar2) - (uint)(uVar11 < uVar1);
          uVar9 = 1;
          goto LAB_ram_0001c920;
        }
      }
      else {
        uVar2 = uVar2 | 0x800000;
        uVar3 = uVar5;
        uVar5 = uVar9;
      }
      if (uVar5 != 0x7ff) {
        if ((int)uVar3 < 0x39) {
          if ((int)uVar3 < 0x20) {
            uVar1 = uVar2 << (0x20 - uVar3 & 0x1f) | uVar12 >> (uVar3 & 0x1f) |
                    (uint)(uVar12 << (0x20 - uVar3 & 0x1f) != 0);
            uVar7 = uVar7 - (uVar2 >> (uVar3 & 0x1f));
          }
          else {
            if (uVar3 != 0x20) {
              uVar12 = uVar12 | uVar2 << (0x40 - uVar3 & 0x1f);
            }
            uVar1 = (uint)(uVar12 != 0) | uVar2 >> (uVar3 - 0x20 & 0x1f);
          }
        }
        else {
          uVar1 = (uint)(uVar2 != 0 || uVar12 != 0);
        }
        uVar1 = uVar11 - uVar1;
        uVar6 = uVar7 - (uVar11 < uVar1);
        goto LAB_ram_0001c920;
      }
      uVar9 = uVar10 << 0x1d | param_3 & 0x1fffffff;
      param_2 = param_4;
      goto LAB_ram_0001cbe8;
    }
LAB_ram_0001caa0:
    if (uVar5 != 0) {
      uVar5 = uVar9 - uVar3;
      if (uVar3 == 0) {
        if (uVar2 == 0 && uVar12 == 0) {
          uVar9 = uVar10 << 0x1d | param_3 & 0x1fffffff;
          goto LAB_ram_0001cb84;
        }
        uVar3 = uVar5 - 1;
        if (uVar3 == 0) {
          uVar6 = uVar2 + uVar7 + (uint)(uVar12 + uVar11 < uVar11);
          goto LAB_ram_0001cd46;
        }
      }
      else {
        uVar2 = uVar2 | 0x800000;
        uVar3 = uVar5;
        uVar5 = uVar9;
      }
      if (uVar5 == 0x7ff) {
LAB_ram_0001ce88:
        uVar9 = uVar10 << 0x1d | param_3 & 0x1fffffff;
        goto LAB_ram_0001cbe8;
      }
      if ((int)uVar3 < 0x39) {
        if ((int)uVar3 < 0x20) {
          uVar1 = uVar2 << (0x20 - uVar3 & 0x1f) | uVar12 >> (uVar3 & 0x1f) |
                  (uint)(uVar12 << (0x20 - uVar3 & 0x1f) != 0);
          uVar7 = uVar7 + (uVar2 >> (uVar3 & 0x1f));
        }
        else {
          if (uVar3 != 0x20) {
            uVar12 = uVar12 | uVar2 << (0x40 - uVar3 & 0x1f);
          }
          uVar1 = (uint)(uVar12 != 0) | uVar2 >> (uVar3 - 0x20 & 0x1f);
        }
      }
      else {
        uVar1 = (uint)(uVar2 != 0 || uVar12 != 0);
      }
      uVar1 = uVar1 + uVar11;
      uVar6 = (uVar1 < uVar11) + uVar7;
LAB_ram_0001cbac:
      uVar3 = uVar9;
      if ((int)(uVar6 << 8) < 0) {
        uVar3 = uVar9 + 1;
        if (uVar3 != 0x7ff) goto LAB_ram_0001cd52;
        goto LAB_ram_0001cbc0;
      }
joined_r0x0001cc0a:
      if ((uVar1 & 7) != 0) goto LAB_ram_0001c98c;
      goto LAB_ram_0001cb72;
    }
    uVar9 = uVar3 + 1;
    if ((uVar9 & 0x7fe) != 0) {
      if (uVar9 != 0x7ff) {
        uVar6 = uVar2 + uVar7 + (uint)(uVar11 + uVar12 < uVar12);
        uVar1 = uVar6 * -0x80000000 | uVar11 + uVar12 >> 1;
        uVar6 = uVar6 >> 1;
        uVar3 = uVar9;
        goto joined_r0x0001cc0a;
      }
      goto LAB_ram_0001cbc0;
    }
    if (uVar3 == 0) {
      uVar5 = 0;
      if (uVar2 == 0 && uVar12 == 0) {
        uVar9 = param_3 & 0x1fffffff | uVar10 << 0x1d;
      }
      else {
        if (uVar7 != 0 || uVar11 != 0) {
          uVar1 = uVar12 + uVar11;
          uVar6 = uVar2 + uVar7 + (uint)(uVar1 < uVar12);
          if ((int)(uVar6 * 0x100) < 0) {
            uVar6 = uVar6 & 0xff7fffff;
            uVar3 = 1;
          }
          goto joined_r0x0001cc0a;
        }
        uVar9 = uVar8 << 0x1d | param_1 & 0x1fffffff;
        uVar6 = uVar1;
      }
      goto LAB_ram_0001cb8c;
    }
    if (uVar2 == 0 && uVar12 == 0) goto LAB_ram_0001ce88;
    if (uVar7 == 0 && uVar11 == 0) goto LAB_ram_0001ce1c;
LAB_ram_0001cbee:
    param_2 = 0;
    uVar5 = 0x7ff;
    uVar6 = 0x80000;
    uVar9 = 0;
  }
  else {
    param_4 = param_4 ^ 1;
    uVar5 = uVar3 - uVar9;
    if (param_2 == param_4) {
      if ((int)uVar5 < 1) goto LAB_ram_0001caa0;
      if (uVar9 != 0) {
        uVar7 = uVar7 | 0x800000;
        uVar6 = uVar5;
        if (uVar3 != 0x7ff) {
LAB_ram_0001ca06:
          if ((int)uVar6 < 0x39) {
            if ((int)uVar6 < 0x20) {
              uVar1 = uVar7 << (0x20 - uVar6 & 0x1f) | uVar11 >> (uVar6 & 0x1f) |
                      (uint)(uVar11 << (0x20 - uVar6 & 0x1f) != 0);
              uVar2 = uVar2 + (uVar7 >> (uVar6 & 0x1f));
            }
            else {
              if (uVar6 != 0x20) {
                uVar11 = uVar11 | uVar7 << (0x40 - uVar6 & 0x1f);
              }
              uVar1 = (uint)(uVar11 != 0) | uVar7 >> (uVar6 - 0x20 & 0x1f);
            }
          }
          else {
            uVar1 = (uint)(uVar7 != 0 || uVar11 != 0);
          }
          uVar1 = uVar1 + uVar12;
          uVar6 = (uVar1 < uVar12) + uVar2;
          uVar9 = uVar3;
          goto LAB_ram_0001cbac;
        }
LAB_ram_0001ccd0:
        uVar9 = param_1 & 0x1fffffff | uVar8 << 0x1d;
        uVar6 = uVar1;
        goto LAB_ram_0001cbe8;
      }
      if (uVar7 == 0 && uVar11 == 0) goto LAB_ram_0001cc36;
      uVar6 = uVar5 - 1;
      if (uVar6 != 0) {
        if (uVar5 != 0x7ff) goto LAB_ram_0001ca06;
LAB_ram_0001ce1c:
        uVar9 = uVar8 << 0x1d | param_1 & 0x1fffffff;
        uVar6 = uVar1;
        goto LAB_ram_0001cbe8;
      }
      uVar6 = uVar2 + uVar7 + (uint)(uVar12 + uVar11 < uVar12);
LAB_ram_0001cd46:
      uVar1 = uVar12 + uVar11;
      uVar3 = 1;
      if (-1 < (int)(uVar6 << 8)) goto joined_r0x0001cc0a;
      uVar3 = 2;
LAB_ram_0001cd52:
      uVar1 = uVar6 << 0x1f | uVar1 >> 1 | uVar1 & 1;
      uVar6 = (uVar6 & 0xff7fffff) >> 1;
LAB_ram_0001c986:
      if ((uVar1 & 7) != 0) {
LAB_ram_0001c98c:
        if ((uVar1 & 0xf) != 4) {
          uVar6 = uVar6 + (uVar1 + 4 < uVar1);
          uVar1 = uVar1 + 4;
        }
      }
      if (-1 < (int)(uVar6 << 8)) {
LAB_ram_0001cb72:
        uVar9 = uVar1 >> 3 | uVar6 << 0x1d;
        uVar5 = uVar3;
        uVar6 = uVar6 >> 3;
        goto LAB_ram_0001cb84;
      }
      if (uVar3 + 1 != 0x7ff) {
        uVar9 = uVar6 << 0x1d | uVar1 >> 3;
        uVar6 = (uVar6 & 0x7fffff) >> 3;
        uVar5 = uVar3 + 1 & 0x7ff;
        goto LAB_ram_0001c9d4;
      }
    }
    else {
      if ((int)uVar5 < 1) goto LAB_ram_0001ca50;
      if (uVar9 != 0) {
        uVar7 = uVar7 | 0x800000;
        uVar6 = uVar5;
        if (uVar3 == 0x7ff) {
          uVar9 = uVar8 << 0x1d | param_1 & 0x1fffffff;
          uVar6 = uVar1;
          goto LAB_ram_0001cbe8;
        }
LAB_ram_0001c8e0:
        if ((int)uVar6 < 0x39) {
          if ((int)uVar6 < 0x20) {
            uVar1 = uVar7 << (0x20 - uVar6 & 0x1f) | uVar11 >> (uVar6 & 0x1f) |
                    (uint)(uVar11 << (0x20 - uVar6 & 0x1f) != 0);
            uVar2 = uVar2 - (uVar7 >> (uVar6 & 0x1f));
          }
          else {
            if (uVar6 != 0x20) {
              uVar11 = uVar11 | uVar7 << (0x40 - uVar6 & 0x1f);
            }
            uVar1 = (uint)(uVar11 != 0) | uVar7 >> (uVar6 - 0x20 & 0x1f);
          }
        }
        else {
          uVar1 = (uint)(uVar7 != 0 || uVar11 != 0);
        }
        uVar1 = uVar12 - uVar1;
        uVar6 = uVar2 - (uVar12 < uVar1);
        param_4 = param_2;
        uVar9 = uVar3;
LAB_ram_0001c920:
        uVar3 = uVar9;
        param_2 = param_4;
        if ((int)(uVar6 << 8) < 0) {
          uVar6 = uVar6 & 0x7fffff;
          goto LAB_ram_0001c932;
        }
        goto joined_r0x0001cc0a;
      }
      if (uVar7 != 0 || uVar11 != 0) {
        uVar6 = uVar5 - 1;
        if (uVar6 != 0) {
          if (uVar5 == 0x7ff) goto LAB_ram_0001ccd0;
          goto LAB_ram_0001c8e0;
        }
        uVar1 = uVar12 + param_3 * -8;
        uVar6 = (uVar2 - uVar7) - (uint)(uVar12 < uVar1);
        uVar9 = 1;
        param_4 = param_2;
        goto LAB_ram_0001c920;
      }
LAB_ram_0001cc36:
      uVar9 = param_1 & 0x1fffffff | uVar8 << 0x1d;
      uVar6 = uVar1;
LAB_ram_0001cb84:
      if (uVar5 != 0x7ff) {
LAB_ram_0001cb8c:
        uVar6 = uVar6 & 0xfffff;
        uVar5 = uVar5 & 0x7ff;
        goto LAB_ram_0001c9d4;
      }
LAB_ram_0001cbe8:
      if (uVar9 != 0 || uVar6 != 0) goto LAB_ram_0001cbee;
    }
LAB_ram_0001cbc0:
    uVar5 = 0x7ff;
    uVar6 = 0;
    uVar9 = 0;
  }
LAB_ram_0001c9d4:
  return CONCAT44(uVar6 | uVar5 << 0x14 | param_2 << 0x1f,uVar9);
}

