/* Original address: 0x0000c570; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 31; DMA transfer-complete flag handling */

void DMA1_Channel5_IRQHandler(void)

{
  gp = &__global_pointer;
  DMA_ClearITPendingBit(0x20000);
  return;
}

