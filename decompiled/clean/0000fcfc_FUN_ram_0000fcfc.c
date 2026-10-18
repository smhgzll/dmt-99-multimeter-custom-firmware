/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000fcfc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000fcfc; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000fcfc(uint param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  gp = &__global_pointer;
  if (param_1 < 0x80) {
    return param_1;
  }
  if (param_2 == 0x3a8) {
    iVar2 = 0x10;
    iVar1 = 0;
    iVar4 = -1;
    do {
      iVar3 = ((uint)(iVar4 - iVar1) >> 1) + iVar1;
      if (*(ushort *)(&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar3) == param_1) {
        return (uint)*(ushort *)
                      ((int)&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar3 * 4 + 2);
      }
      if (*(ushort *)(&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar3) < param_1) {
        iVar1 = iVar3;
        iVar3 = iVar4;
      }
      iVar2 = iVar2 + -1;
      iVar4 = iVar3;
    } while (iVar2 != 0);
  }
  return 0;
}

