/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000090fc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000090fc; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_000090fc(uint param_1)

{
  int iVar1;
  
  gp = &__global_pointer;
  iVar1 = 1;
  for (; 0xf < param_1; param_1 = param_1 >> 4) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}

