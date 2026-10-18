/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001e7c2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001e7c2; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0001e7c2(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  gp = &__global_pointer;
  DAT_ram_200054d4 = 0;
  iVar1 = FUN_ram_0001a720(param_2);
  if ((iVar1 == -1) && (DAT_ram_200054d4 != 0)) {
    *param_1 = DAT_ram_200054d4;
  }
  return;
}

