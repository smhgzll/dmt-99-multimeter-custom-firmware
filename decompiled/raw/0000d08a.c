/* Original address: 0x0000d08a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM status complement-mask write */

void TIM_ClearITPendingBit(int param_1,ushort param_2)

{
  gp = &__global_pointer;
  *(ushort *)(param_1 + 0x10) = ~param_2;
  return;
}

