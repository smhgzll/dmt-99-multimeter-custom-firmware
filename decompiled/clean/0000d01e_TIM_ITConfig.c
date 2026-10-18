/* Semantic review: high. TIM interrupt DMA enable register mask.
 * Address 0x0000d01e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d01e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM interrupt DMA enable register mask */

void TIM_ITConfig(int param_1,ushort param_2,int param_3)

{
  gp = &__global_pointer;
  if (param_3 == 0) {
    param_2 = ~param_2 & *(ushort *)(param_1 + 0xc);
  }
  else {
    param_2 = param_2 | *(ushort *)(param_1 + 0xc);
  }
  *(ushort *)(param_1 + 0xc) = param_2;
  return;
}

