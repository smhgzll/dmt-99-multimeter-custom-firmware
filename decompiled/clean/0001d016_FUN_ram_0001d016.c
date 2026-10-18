/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001d016; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001d016; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001d016(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = param_2 >> 0x14 & 0x7ff;
  uVar1 = 0;
  if (uVar2 < 0x3ff) {
    return uVar1;
  }
  if ((-1 < (int)param_2) && (uVar1 = 0xffffffff, uVar2 < 0x41f)) {
    uVar1 = param_2 & 0xfffff | 0x100000;
    if ((int)(0x433 - uVar2) < 0x20) {
      return param_1 >> (0x433 - uVar2 & 0x1f) | uVar1 << (uVar2 - 0x413 & 0x1f);
    }
    return uVar1 >> (0x413 - uVar2 & 0x1f);
  }
  return uVar1;
}

