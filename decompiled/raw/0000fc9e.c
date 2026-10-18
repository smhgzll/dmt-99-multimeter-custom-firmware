/* Original address: 0x0000fc9e; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000fc9e(uint param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  gp = &__global_pointer;
  if (param_1 < 0x80) {
    return param_1 & 0xffff;
  }
  uVar1 = 0;
  if ((param_1 < 0x10000) && (param_2 == 0x3a8)) {
    iVar4 = 0x10;
    iVar6 = -1;
    iVar2 = 0;
    while( true ) {
      iVar5 = ((uint)(iVar6 - iVar2) >> 1) + iVar2;
      if ((uint)*(ushort *)(&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar5) ==
          (param_1 & 0xffff)) break;
      iVar3 = iVar5;
      if ((param_1 & 0xffff) <=
          (uint)*(ushort *)(&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar5)) {
        iVar3 = iVar2;
        iVar6 = iVar5;
      }
      iVar4 = iVar4 + -1;
      iVar2 = iVar3;
      if (iVar4 == 0) {
        return 0;
      }
    }
    uVar1 = (uint)*(ushort *)((int)&switchD_ram:0000fe20::switchdataD_ram_00023fe8 + iVar5 * 4 + 2);
  }
  return uVar1;
}

