/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00006838; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00006838; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_00006838(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = param_2 + 3U & 0xfffffffc;
  if (uVar2 <= *(uint *)(param_1 + 0x70)) {
    *(uint *)(param_1 + 0x70) = *(uint *)(param_1 + 0x70) - uVar2;
    iVar1 = *(int *)(param_1 + 0x6c);
    *(uint *)(param_1 + 0x6c) = uVar2 + iVar1;
    return iVar1;
  }
  return 0;
}

