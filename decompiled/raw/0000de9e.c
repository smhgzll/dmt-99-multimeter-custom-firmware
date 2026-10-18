/* Original address: 0x0000de9e; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000de9e(byte *param_1)

{
  byte *pbVar1;
  byte *pbVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  uVar3 = 0;
  pbVar1 = param_1;
  do {
    pbVar2 = pbVar1 + 1;
    uVar3 = (uVar3 << 7 | uVar3 >> 1) + (uint)*pbVar1 & 0xff;
    pbVar1 = pbVar2;
  } while (pbVar2 != param_1 + 0xb);
  return uVar3;
}

