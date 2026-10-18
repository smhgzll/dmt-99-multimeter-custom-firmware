/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001d14c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001d14c; Ghidra pseudocode, not compilable reconstructed source. */

undefined8 FUN_ram_0001d14c(uint param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  gp = &__global_pointer;
  uVar4 = param_2 >> 0x14 & 0x7ff;
  if (uVar4 < 0x3ff) {
    param_1 = 0;
    uVar3 = 0;
  }
  else {
    if (0x43d < uVar4) {
      iVar2 = ((int)param_2 >> 0x1f) + 1;
      iVar1 = -iVar2;
      return CONCAT44((-0x80000000 - (iVar2 >> 0x1f)) - (uint)(iVar1 != 0),iVar1);
    }
    uVar3 = param_2 & 0xfffff | 0x100000;
    if (uVar4 < 0x433) {
      uVar5 = 0x433 - uVar4;
      if ((int)uVar5 < 0x20) {
        param_1 = param_1 >> (uVar5 & 0x1f) | uVar3 << (uVar4 - 0x413 & 0x1f);
        uVar3 = uVar3 >> (uVar5 & 0x1f);
      }
      else {
        param_1 = uVar3 >> (0x413 - uVar4 & 0x1f);
        uVar3 = 0;
      }
    }
    else {
      uVar5 = uVar4 - 0x433;
      if ((int)(uVar4 - 0x453) < 0) {
        uVar3 = uVar3 << (uVar5 & 0x1f) | (param_1 >> 1) >> (0x1f - uVar5 & 0x1f);
        param_1 = param_1 << (uVar5 & 0x1f);
      }
      else {
        uVar3 = param_1 << (uVar4 - 0x453 & 0x1f);
        param_1 = 0;
      }
    }
    if ((int)param_2 < 0) {
      return CONCAT44(-(uint)(-param_1 != 0) - uVar3,-param_1);
    }
  }
  return CONCAT44(uVar3,param_1);
}

