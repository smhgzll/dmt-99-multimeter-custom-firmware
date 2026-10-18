/* Original address: 0x0000d04a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM CHCTLR1 bit 3 output compare preload */

void TIM_OC1PreloadConfig(int param_1,ushort param_2)

{
  gp = &__global_pointer;
  *(ushort *)(param_1 + 0x18) = param_2 | *(ushort *)(param_1 + 0x18) & 0xfff7;
  return;
}

