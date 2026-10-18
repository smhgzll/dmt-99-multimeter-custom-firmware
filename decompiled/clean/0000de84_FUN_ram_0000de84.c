/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000de84; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000de84; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0000de84(int param_1,int param_2)

{
  gp = &__global_pointer;
  if (param_2 - 2U < *(int *)(param_1 + 0x1c) - 2U) {
    return *(int *)(param_1 + 0x30) + (uint)*(ushort *)(param_1 + 10) * (param_2 - 2U);
  }
  return 0;
}

