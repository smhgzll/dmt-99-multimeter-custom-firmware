/* Original address: 0x0001b294; Ghidra pseudocode, not compilable reconstructed source. */

ulonglong FUN_ram_0001b294(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  
  gp = &__global_pointer;
  if (param_4 != 0) {
    if (param_4 <= param_2) {
      if (param_4 < 0x10000) {
        if (param_4 < 0x100) {
          iVar5 = 0;
          uVar2 = param_4;
        }
        else {
          iVar5 = 8;
          uVar2 = param_4 >> 8;
        }
      }
      else if (param_4 < 0x1000000) {
        iVar5 = 0x10;
        uVar2 = param_4 >> 0x10;
      }
      else {
        iVar5 = 0x18;
        uVar2 = param_4 >> 0x18;
      }
      uVar9 = (uint)(byte)(&DAT_ram_000270d0)[uVar2] + iVar5;
      uVar2 = 0x20 - uVar9;
      if (uVar2 != 0) {
        uVar6 = param_4 << (uVar2 & 0x1f) | param_3 >> (uVar9 & 0x1f);
        uVar1 = param_2 >> (uVar9 & 0x1f);
        uVar7 = uVar6 >> 0x10;
        uVar10 = uVar1 / uVar7;
        uVar3 = param_2 << (uVar2 & 0x1f) | param_1 >> (uVar9 & 0x1f);
        param_3 = param_3 << (uVar2 & 0x1f);
        param_1 = param_1 << (uVar2 & 0x1f);
        uVar4 = (uVar6 & 0xffff) * uVar10;
        uVar8 = uVar1 % uVar7 << 0x10 | uVar3 >> 0x10;
        uVar1 = uVar10;
        if (uVar8 < uVar4) {
          uVar8 = uVar8 + uVar6;
          uVar1 = uVar10 - 1;
          if ((uVar6 <= uVar8) && (uVar8 < uVar4)) {
            uVar1 = uVar10 - 2;
            uVar8 = uVar8 + uVar6;
          }
        }
        uVar10 = (uVar8 - uVar4) / uVar7;
        uVar11 = (uVar6 & 0xffff) * uVar10;
        uVar7 = (uVar8 - uVar4) % uVar7 << 0x10 | uVar3 & 0xffff;
        uVar3 = uVar10;
        if (uVar7 < uVar11) {
          uVar7 = uVar7 + uVar6;
          uVar3 = uVar10 - 1;
          if ((uVar6 <= uVar7) && (uVar7 < uVar11)) {
            uVar3 = uVar10 - 2;
            uVar7 = uVar7 + uVar6;
          }
        }
        uVar8 = (uVar1 << 0x10 | uVar3) >> 0x10;
        uVar10 = (uVar3 & 0xffff) * (param_3 & 0xffff);
        uVar7 = uVar7 - uVar11;
        uVar4 = uVar8 * (param_3 & 0xffff);
        uVar1 = (uVar10 >> 0x10) + (uVar3 & 0xffff) * (param_3 >> 0x10) + uVar4;
        iVar5 = uVar8 * (param_3 >> 0x10);
        if (uVar1 < uVar4) {
          iVar5 = iVar5 + 0x10000;
        }
        uVar3 = (uVar1 >> 0x10) + iVar5;
        uVar1 = uVar1 * 0x10000 + (uVar10 & 0xffff);
        if ((uVar7 < uVar3) || ((uVar4 = uVar1, uVar7 == uVar3 && (param_1 < uVar1)))) {
          uVar4 = uVar1 - param_3;
          uVar3 = uVar3 - ((uVar1 < uVar4) + uVar6);
        }
        uVar1 = (uVar7 - uVar3) - (uint)(param_1 < param_1 - uVar4);
        return CONCAT44(uVar1 >> (uVar2 & 0x1f),
                        uVar1 << (uVar9 & 0x1f) | param_1 - uVar4 >> (uVar2 & 0x1f));
      }
      if ((param_4 < param_2) || (uVar2 = param_1, param_3 <= param_1)) {
        uVar2 = param_1 - param_3;
        param_2 = (param_2 - param_4) - (uint)(param_1 < uVar2);
      }
      return CONCAT44(param_2,uVar2);
    }
LAB_ram_0001b340:
    return CONCAT44(param_2,param_1);
  }
  if (param_2 < param_3) {
    if (param_3 < 0x10000) {
      iVar5 = 0;
      uVar2 = param_3;
      if (0xff < param_3) {
        uVar2 = param_3 >> 8;
        iVar5 = 8;
      }
    }
    else if (param_3 < 0x1000000) {
      uVar2 = param_3 >> 0x10;
      iVar5 = 0x10;
    }
    else {
      uVar2 = param_3 >> 0x18;
      iVar5 = 0x18;
    }
    uVar9 = 0x20 - (iVar5 + (uint)(byte)(&DAT_ram_000270d0)[uVar2]);
    if (uVar9 != 0) {
      param_3 = param_3 << (uVar9 & 0x1f);
      param_2 = param_1 >> (iVar5 + (uint)(byte)(&DAT_ram_000270d0)[uVar2] & 0x1f) |
                param_2 << (uVar9 & 0x1f);
      param_1 = param_1 << (uVar9 & 0x1f);
    }
    uVar2 = param_3 >> 0x10;
    uVar6 = (param_3 & 0xffff) * (param_2 / uVar2);
    uVar3 = param_2 % uVar2 << 0x10 | param_1 >> 0x10;
    if (((uVar3 < uVar6) && (uVar3 = uVar3 + param_3, param_3 <= uVar3)) && (uVar3 < uVar6)) {
      uVar3 = uVar3 + param_3;
    }
    uVar1 = (param_3 & 0xffff) * ((uVar3 - uVar6) / uVar2);
    uVar2 = param_1 & 0xffff | (uVar3 - uVar6) % uVar2 << 0x10;
    if (uVar1 <= uVar2) {
      param_1 = uVar2 - uVar1 >> (uVar9 & 0x1f);
      param_2 = 0;
      goto LAB_ram_0001b340;
    }
  }
  else {
    if (param_3 == 0) {
      param_3 = 1 / 0;
    }
    if (param_3 < 0x10000) {
      iVar5 = 0;
      uVar2 = param_3;
      if (0xff < param_3) {
        uVar2 = param_3 >> 8;
        iVar5 = 8;
      }
    }
    else if (param_3 < 0x1000000) {
      iVar5 = 0x10;
      uVar2 = param_3 >> 0x10;
    }
    else {
      iVar5 = 0x18;
      uVar2 = param_3 >> 0x18;
    }
    uVar2 = iVar5 + (uint)(byte)(&DAT_ram_000270d0)[uVar2];
    uVar9 = 0x20 - uVar2;
    if (uVar9 == 0) {
      param_2 = param_2 - param_3;
      uVar3 = param_3 >> 0x10;
      uVar1 = param_3 & 0xffff;
    }
    else {
      param_3 = param_3 << (uVar9 & 0x1f);
      uVar6 = param_2 >> (uVar2 & 0x1f);
      uVar3 = param_3 >> 0x10;
      uVar1 = param_3 & 0xffff;
      uVar2 = param_2 << (uVar9 & 0x1f) | param_1 >> (uVar2 & 0x1f);
      param_1 = param_1 << (uVar9 & 0x1f);
      uVar7 = uVar1 * (uVar6 / uVar3);
      uVar6 = uVar6 % uVar3 << 0x10 | uVar2 >> 0x10;
      if (((uVar6 < uVar7) && (uVar6 = uVar6 + param_3, param_3 <= uVar6)) && (uVar6 < uVar7)) {
        uVar6 = uVar6 + param_3;
      }
      uVar4 = uVar1 * ((uVar6 - uVar7) / uVar3);
      param_2 = (uVar6 - uVar7) % uVar3 << 0x10 | uVar2 & 0xffff;
      if (((param_2 < uVar4) && (param_2 = param_2 + param_3, param_3 <= param_2)) &&
         (param_2 < uVar4)) {
        param_2 = param_2 + param_3;
      }
      param_2 = param_2 - uVar4;
    }
    uVar2 = (param_2 / uVar3) * uVar1;
    uVar6 = param_1 >> 0x10 | param_2 % uVar3 << 0x10;
    if (((uVar6 < uVar2) && (uVar6 = uVar6 + param_3, param_3 <= uVar6)) && (uVar6 < uVar2)) {
      uVar6 = uVar6 + param_3;
    }
    uVar1 = ((uVar6 - uVar2) / uVar3) * uVar1;
    uVar2 = (uVar6 - uVar2) % uVar3 << 0x10 | param_1 & 0xffff;
    if (uVar1 <= uVar2) goto LAB_ram_0001b3d4;
  }
  uVar2 = uVar2 + param_3;
  if ((param_3 <= uVar2) && (uVar2 < uVar1)) {
    uVar2 = uVar2 + param_3;
  }
LAB_ram_0001b3d4:
  return (ulonglong)(uVar2 - uVar1 >> (uVar9 & 0x1f));
}

