/* Original address: 0x0001e38e; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001e38e(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  uVar2 = param_2 >> 0x14 & 0x7ff;
  uVar1 = param_2 >> 0x1f;
  uVar3 = param_1 >> 0x1d | (param_2 & 0xfffff) << 3;
  uVar6 = param_1 << 3;
  if ((uVar2 + 1 & 0x7fe) == 0) {
    if (uVar2 != 0) {
      if (uVar3 != 0 || uVar6 != 0) {
        return 0x7fc00000;
      }
      goto LAB_ram_0001e3c0;
    }
    if (uVar3 == 0 && uVar6 == 0) {
      return uVar1 << 0x1f;
    }
LAB_ram_0001e436:
    uVar5 = 0;
    uVar3 = 0;
  }
  else {
    uVar5 = uVar2 - 0x380;
    if (0xfe < (int)uVar5) goto LAB_ram_0001e3c0;
    if ((int)uVar5 < 1) {
      if ((int)uVar5 < -0x17) goto LAB_ram_0001e436;
      uVar4 = 0x1e - uVar5;
      uVar3 = uVar3 | 0x800000;
      if ((int)uVar4 < 0x20) {
        uVar3 = uVar6 >> (uVar4 & 0x1f) |
                (uint)(uVar6 << (uVar2 - 0x37e & 0x1f) != 0) | uVar3 << (uVar2 - 0x37e & 0x1f);
      }
      else {
        if (uVar4 != 0x20) {
          uVar6 = uVar6 | uVar3 << (uVar2 - 0x35e & 0x1f);
        }
        uVar3 = (uint)(uVar6 != 0) | uVar3 >> (-uVar5 - 2 & 0x1f);
      }
      uVar5 = 0;
      if ((uVar3 & 7) == 0) {
        uVar2 = 1;
        if (-1 < (int)(uVar3 << 5)) {
          uVar5 = 0;
          goto LAB_ram_0001e438;
        }
        goto LAB_ram_0001e41a;
      }
    }
    else {
      uVar2 = (uint)((param_1 & 0x3ffffff) != 0);
      uVar6 = (param_1 & 0x1fffffff) >> 0x1a;
      uVar3 = uVar2 | uVar3 << 3 | uVar6;
      if (uVar2 == 0 && uVar6 == 0) goto LAB_ram_0001e438;
    }
    if ((uVar3 & 0xf) != 4) {
      uVar3 = uVar3 + 4;
    }
    if ((uVar3 & 0x4000000) != 0) {
      uVar2 = uVar5 + 1 & 0xff;
      if (uVar5 + 1 != 0xff) {
LAB_ram_0001e41a:
        return (uVar3 & 0x3ffffff) >> 3 | uVar2 << 0x17 | uVar1 << 0x1f;
      }
LAB_ram_0001e3c0:
      return uVar1 << 0x1f | 0x7f800000;
    }
  }
LAB_ram_0001e438:
  return (uVar3 & 0x3ffffff) >> 3 | (uVar5 & 0xff) << 0x17 | uVar1 << 0x1f;
}

