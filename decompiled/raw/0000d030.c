/* Original address: 0x0000d030; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM CTLR1 bit 7 auto-reload preload enable */

void TIM_ARRPreloadConfig(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xff7f;
  }
  else {
    uVar1 = *param_1 | 0x80;
  }
  *param_1 = uVar1;
  return;
}

