/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001e164; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001e164; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001e164(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = param_1 >> 0x17 & 0xff;
  if (uVar2 < 0x7f) {
    uVar2 = 0;
  }
  else {
    if (0x9d < uVar2) {
      return -((int)param_1 >> 0x1f) + 0x7fffffff;
    }
    uVar1 = param_1 & 0x7fffff | 0x800000;
    if (uVar2 < 0x96) {
      uVar1 = uVar1 >> (0x96 - uVar2 & 0x1f);
    }
    else {
      uVar1 = uVar1 << (uVar2 - 0x96 & 0x1f);
    }
    uVar2 = -uVar1;
    if (-((int)param_1 >> 0x1f) == 0) {
      return uVar1;
    }
  }
  return uVar2;
}

