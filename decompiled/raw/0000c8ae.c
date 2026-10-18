/* Original address: 0x0000c8ae; Ghidra pseudocode, not compilable reconstructed source. */

/* high: DMA CFGR interrupt mask enable */

void DMA_ITConfig(uint *param_1,uint param_2,int param_3)

{
  gp = &__global_pointer;
  if (param_3 == 0) {
    param_2 = ~param_2 & *param_1;
  }
  else {
    param_2 = param_2 | *param_1;
  }
  *param_1 = param_2;
  return;
}

