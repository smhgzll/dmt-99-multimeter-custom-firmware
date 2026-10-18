/* Semantic review: high. Vector slot 31; DMA transfer-complete flag handling.
 * Address 0x0000c570; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c570; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 31; DMA transfer-complete flag handling */

void DMA1_Channel5_IRQHandler(void)

{
  gp = &__global_pointer;
  DMA_ClearITPendingBit(0x20000);
  return;
}

