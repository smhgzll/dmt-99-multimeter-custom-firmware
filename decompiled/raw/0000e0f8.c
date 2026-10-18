/* Original address: 0x0000e0f8; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000e0f8(int param_1,int param_2)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = (uint)*(ushort *)(param_2 + 0x1a);
  if (param_1 == 3) {
    uVar1 = CONCAT22(*(undefined2 *)(param_2 + 0x14),*(ushort *)(param_2 + 0x1a));
  }
  return uVar1;
}

