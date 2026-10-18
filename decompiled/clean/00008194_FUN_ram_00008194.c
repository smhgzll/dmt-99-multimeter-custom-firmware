/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00008194; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00008194; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00008194(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                     undefined4 param_5)

{
  gp = &__global_pointer;
  FUN_ram_00008050();
  FUN_ram_00008050(param_1,param_2,param_1,param_4,param_5);
  FUN_ram_00008050(param_1,param_4,param_3,param_4,param_5);
  FUN_ram_00008050(param_3,param_2,param_3,param_4,param_5);
  return;
}

