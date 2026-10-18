/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00008588; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00008588; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00008588(uint param_1,undefined4 param_2,char *param_3,undefined4 param_4,
                     undefined4 param_5,uint param_6,undefined4 param_7)

{
  gp = &__global_pointer;
  for (; (byte)(*param_3 - 0x20U) < 0x5f; param_3 = param_3 + 1) {
    FUN_ram_000083d6(param_1,param_2,*param_3,param_6,param_4,param_5,param_7);
    param_1 = param_1 + (param_6 >> 1) & 0xffff;
  }
  return;
}

