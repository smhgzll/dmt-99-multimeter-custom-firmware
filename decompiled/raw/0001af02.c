/* Original address: 0x0001af02; Ghidra pseudocode, not compilable reconstructed source. */

ulonglong FUN_ram_0001af02(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  gp = &__global_pointer;
  if (param_4 == 0) {
    if (param_3 <= param_2) {
      if (param_3 == 0) {
        param_3 = 1 / 0;
      }
      if (param_3 < 0x10000) {
        iVar7 = 0;
        uVar8 = param_3;
        if (0xff < param_3) {
          uVar8 = param_3 >> 8;
          iVar7 = 8;
        }
      }
      else if (param_3 < 0x1000000) {
        iVar7 = 0x10;
        uVar8 = param_3 >> 0x10;
      }
      else {
        iVar7 = 0x18;
        uVar8 = param_3 >> 0x18;
      }
      uVar8 = (uint)(byte)(&DAT_ram_000270d0)[uVar8] + iVar7;
      uVar4 = 0x20 - uVar8;
      if (uVar4 == 0) {
        param_2 = param_2 - param_3;
        uVar10 = param_3 >> 0x10;
        uVar5 = param_3 & 0xffff;
        uVar4 = 1;
      }
      else {
        param_3 = param_3 << (uVar4 & 0x1f);
        uVar1 = param_2 >> (uVar8 & 0x1f);
        uVar10 = param_3 >> 0x10;
        uVar3 = uVar1 / uVar10;
        uVar2 = param_2 << (uVar4 & 0x1f) | param_1 >> (uVar8 & 0x1f);
        uVar5 = param_3 & 0xffff;
        param_1 = param_1 << (uVar4 & 0x1f);
        uVar9 = uVar5 * uVar3;
        uVar4 = uVar1 % uVar10 << 0x10 | uVar2 >> 0x10;
        uVar8 = uVar3;
        if (uVar4 < uVar9) {
          uVar4 = uVar4 + param_3;
          uVar8 = uVar3 - 1;
          if ((param_3 <= uVar4) && (uVar4 < uVar9)) {
            uVar8 = uVar3 - 2;
            uVar4 = uVar4 + param_3;
          }
        }
        uVar3 = (uVar4 - uVar9) / uVar10;
        uVar1 = uVar5 * uVar3;
        param_2 = (uVar4 - uVar9) % uVar10 << 0x10 | uVar2 & 0xffff;
        uVar4 = uVar3;
        if (param_2 < uVar1) {
          param_2 = param_2 + param_3;
          uVar4 = uVar3 - 1;
          if ((param_3 <= param_2) && (param_2 < uVar1)) {
            uVar4 = uVar3 - 2;
            param_2 = param_2 + param_3;
          }
        }
        param_2 = param_2 - uVar1;
        uVar4 = uVar8 << 0x10 | uVar4;
      }
      uVar1 = param_2 / uVar10;
      uVar2 = uVar1 * uVar5;
      uVar9 = param_1 >> 0x10 | param_2 % uVar10 << 0x10;
      uVar8 = uVar1;
      if (uVar9 < uVar2) {
        uVar9 = uVar9 + param_3;
        uVar8 = uVar1 - 1;
        if ((param_3 <= uVar9) && (uVar9 < uVar2)) {
          uVar8 = uVar1 - 2;
          uVar9 = uVar9 + param_3;
        }
      }
      uVar3 = (uVar9 - uVar2) / uVar10;
      uVar2 = (uVar9 - uVar2) % uVar10 << 0x10 | param_1 & 0xffff;
      uVar1 = uVar3;
      if (uVar2 < uVar3 * uVar5) {
        uVar2 = uVar2 + param_3;
        if ((uVar2 < param_3) || (uVar1 = uVar3 - 2, uVar3 * uVar5 <= uVar2)) {
          uVar1 = uVar3 - 1;
        }
      }
      return CONCAT44(uVar4,uVar8 << 0x10 | uVar1);
    }
    if (param_3 < 0x10000) {
      iVar7 = 0;
      uVar8 = param_3;
      if (0xff < param_3) {
        uVar8 = param_3 >> 8;
        iVar7 = 8;
      }
    }
    else if (param_3 < 0x1000000) {
      iVar7 = 0x10;
      uVar8 = param_3 >> 0x10;
    }
    else {
      iVar7 = 0x18;
      uVar8 = param_3 >> 0x18;
    }
    uVar4 = 0x20 - (iVar7 + (uint)(byte)(&DAT_ram_000270d0)[uVar8]);
    if (uVar4 != 0) {
      param_3 = param_3 << (uVar4 & 0x1f);
      param_2 = param_1 >> (iVar7 + (uint)(byte)(&DAT_ram_000270d0)[uVar8] & 0x1f) |
                param_2 << (uVar4 & 0x1f);
      param_1 = param_1 << (uVar4 & 0x1f);
    }
    uVar2 = param_3 >> 0x10;
    uVar4 = param_2 / uVar2;
    uVar1 = (param_3 & 0xffff) * uVar4;
    uVar9 = param_2 % uVar2 << 0x10 | param_1 >> 0x10;
    uVar8 = uVar4;
    if (uVar9 < uVar1) {
      uVar9 = uVar9 + param_3;
      uVar8 = uVar4 - 1;
      if ((param_3 <= uVar9) && (uVar9 < uVar1)) {
        uVar8 = uVar4 - 2;
        uVar9 = uVar9 + param_3;
      }
    }
    uVar5 = (uVar9 - uVar1) / uVar2;
    uVar3 = (param_3 & 0xffff) * uVar5;
    uVar1 = (uVar9 - uVar1) % uVar2 << 0x10 | param_1 & 0xffff;
    uVar4 = uVar5;
    if (uVar1 < uVar3) {
      uVar1 = uVar1 + param_3;
      if ((uVar1 < param_3) || (uVar4 = uVar5 - 2, uVar3 <= uVar1)) {
        uVar4 = uVar5 - 1;
      }
    }
    return (ulonglong)(uVar8 << 0x10 | uVar4);
  }
  if (param_2 < param_4) {
    return 0;
  }
  if (param_4 < 0x10000) {
    if (param_4 < 0x100) {
      iVar7 = 0;
      uVar8 = param_4;
    }
    else {
      iVar7 = 8;
      uVar8 = param_4 >> 8;
    }
  }
  else if (param_4 < 0x1000000) {
    iVar7 = 0x10;
    uVar8 = param_4 >> 0x10;
  }
  else {
    iVar7 = 0x18;
    uVar8 = param_4 >> 0x18;
  }
  uVar8 = (uint)(byte)(&DAT_ram_000270d0)[uVar8] + iVar7;
  uVar4 = 0x20 - uVar8;
  if (uVar4 == 0) {
    if (param_4 < param_2) {
      return 1;
    }
    return (ulonglong)(param_1 < param_3 ^ 1);
  }
  uVar9 = param_4 << (uVar4 & 0x1f) | param_3 >> (uVar8 & 0x1f);
  uVar1 = param_2 >> (uVar8 & 0x1f);
  uVar10 = uVar9 >> 0x10;
  uVar3 = uVar1 / uVar10;
  uVar5 = param_1 >> (uVar8 & 0x1f) | param_2 << (uVar4 & 0x1f);
  param_3 = param_3 << (uVar4 & 0x1f);
  uVar2 = (uVar9 & 0xffff) * uVar3;
  uVar1 = uVar1 % uVar10 << 0x10 | uVar5 >> 0x10;
  uVar8 = uVar3;
  if (uVar1 < uVar2) {
    uVar1 = uVar1 + uVar9;
    uVar8 = uVar3 - 1;
    if ((uVar9 <= uVar1) && (uVar1 < uVar2)) {
      uVar8 = uVar3 - 2;
      uVar1 = uVar1 + uVar9;
    }
  }
  uVar6 = (uVar1 - uVar2) / uVar10;
  uVar3 = (uVar9 & 0xffff) * uVar6;
  uVar2 = (uVar1 - uVar2) % uVar10 << 0x10 | uVar5 & 0xffff;
  uVar1 = uVar6;
  if (uVar2 < uVar3) {
    uVar2 = uVar2 + uVar9;
    uVar1 = uVar6 - 1;
    if ((uVar9 <= uVar2) && (uVar2 < uVar3)) {
      uVar1 = uVar6 - 2;
      uVar2 = uVar2 + uVar9;
    }
  }
  uVar5 = uVar8 << 0x10 | uVar1;
  uVar9 = param_3 & 0xffff;
  param_3 = param_3 >> 0x10;
  uVar10 = (uVar1 & 0xffff) * uVar9;
  uVar9 = (uVar5 >> 0x10) * uVar9;
  uVar8 = (uVar1 & 0xffff) * param_3 + uVar9 + (uVar10 >> 0x10);
  iVar7 = (uVar5 >> 0x10) * param_3;
  if (uVar8 < uVar9) {
    iVar7 = iVar7 + 0x10000;
  }
  uVar1 = iVar7 + (uVar8 >> 0x10);
  if ((uVar1 <= uVar2 - uVar3) &&
     ((uVar2 - uVar3 != uVar1 || (uVar8 * 0x10000 + (uVar10 & 0xffff) <= param_1 << (uVar4 & 0x1f)))
     )) {
    return (ulonglong)uVar5;
  }
  return (ulonglong)(uVar5 - 1);
}

