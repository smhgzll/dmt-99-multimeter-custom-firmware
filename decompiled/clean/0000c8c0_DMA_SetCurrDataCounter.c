/* Semantic review: high. DMA CNTR write.
 * Address 0x0000c8c0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c8c0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: DMA CNTR write */

void DMA_SetCurrDataCounter(int param_1,undefined4 param_2)

{
  gp = &__global_pointer;
  *(undefined4 *)(param_1 + 4) = param_2;
  return;
}

