/* Original address: 0x0001d9da; Ghidra pseudocode, not compilable reconstructed source. */

bool FUN_ram_0001d9da(uint param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  uVar3 = param_1 >> 0x17 & 0xff;
  uVar4 = param_1 & 0x7fffff;
  uVar2 = param_2 >> 0x17 & 0xff;
  if ((uVar3 == 0xff) && (uVar4 != 0)) {
    return true;
  }
  if ((uVar2 == 0xff) && ((param_2 & 0x7fffff) != 0)) {
    return true;
  }
  bVar1 = true;
  if ((((uVar3 == uVar2) && (uVar4 == (param_2 & 0x7fffff))) &&
      (bVar1 = false, param_1 >> 0x1f != param_2 >> 0x1f)) && (bVar1 = true, uVar3 == 0)) {
    return uVar4 != 0;
  }
  return bVar1;
}

