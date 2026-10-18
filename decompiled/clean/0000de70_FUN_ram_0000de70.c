/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000de70; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000de70; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0000de70(undefined1 *param_1,undefined4 param_2)

{
  gp = &__global_pointer;
  *param_1 = (char)param_2;
  param_1[1] = (char)((uint)param_2 >> 8);
  param_1[2] = (char)((uint)param_2 >> 0x10);
  param_1[3] = (char)((uint)param_2 >> 0x18);
  return;
}

