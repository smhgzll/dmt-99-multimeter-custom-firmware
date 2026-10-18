/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001e2e2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001e2e2; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001e2e2(uint param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  uVar3 = param_1 >> 0x17 & 0xff;
  uVar1 = param_1 & 0x7fffff;
  if ((uVar3 + 1 & 0xfe) == 0) {
    if (uVar3 == 0) {
      if (uVar1 == 0) {
        uVar3 = 0;
        uVar4 = 0;
      }
      else {
        iVar2 = FUN_ram_0001e4f8(uVar1);
        if (iVar2 < 0xb) {
          uVar4 = uVar1 >> (0xbU - iVar2 & 0x1f);
          uVar1 = uVar1 << (iVar2 + 0x15U & 0x1f);
        }
        else {
          uVar4 = uVar1 << (iVar2 - 0xbU & 0x1f);
          uVar1 = 0;
        }
        uVar4 = uVar4 & 0xfffff;
        uVar3 = 0x389U - iVar2 & 0x7ff;
      }
    }
    else if (uVar1 == 0) {
      uVar3 = 0x7ff;
      uVar4 = 0;
    }
    else {
      uVar4 = uVar1 >> 3;
      uVar1 = param_1 << 0x1d;
      uVar4 = uVar4 | 0x80000;
      uVar3 = 0x7ff;
    }
  }
  else {
    uVar4 = uVar1 >> 3;
    uVar3 = uVar3 + 0x380;
    uVar1 = param_1 << 0x1d;
  }
  return CONCAT44(uVar4 | uVar3 << 0x14 | param_1 & 0x80000000,uVar1);
}

