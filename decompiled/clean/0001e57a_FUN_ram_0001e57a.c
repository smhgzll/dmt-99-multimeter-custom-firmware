/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001e57a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001e57a; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001e57a(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  byte *pbVar3;
  
  gp = &__global_pointer;
  iVar2 = 0;
  do {
    if (param_3 == iVar2) {
      return 0;
    }
    pbVar3 = (byte *)(param_1 + iVar2);
    iVar2 = iVar2 + 1;
    uVar1 = (uint)*(byte *)(param_2 + iVar2 + -1);
  } while (*pbVar3 == uVar1);
  return *pbVar3 - uVar1;
}

