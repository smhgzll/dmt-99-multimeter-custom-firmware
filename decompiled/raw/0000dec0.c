/* Original address: 0x0000dec0; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000dec0(undefined4 *param_1)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar5;
  byte *pbVar4;
  
  gp = &__global_pointer;
  pbVar5 = (byte *)*param_1;
  pbVar4 = pbVar5;
  if (pbVar5 != (byte *)0x0) {
    do {
      pbVar3 = pbVar4 + 1;
      bVar1 = *pbVar4;
      if (bVar1 < 0x20) {
        return 0;
      }
      pbVar4 = pbVar3;
    } while (bVar1 != 0x3a);
    uVar2 = (int)(char)*pbVar5 - 0x30;
    if ((uVar2 & 0xff) < 10) {
      if (pbVar3 != pbVar5 + 2) {
        return 0xffffffff;
      }
      if ((int)uVar2 < 2) {
        *param_1 = pbVar3;
        return uVar2;
      }
    }
  }
  return 0xffffffff;
}

