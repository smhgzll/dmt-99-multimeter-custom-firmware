/* Semantic review: high. USART CTLR3 DMA mask.
 * Address 0x0000d19e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d19e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART CTLR3 DMA mask */

void USART_DMACmd(int param_1,ushort param_2,int param_3)

{
  gp = &__global_pointer;
  if (param_3 == 0) {
    param_2 = ~param_2 & *(ushort *)(param_1 + 0x14);
  }
  else {
    param_2 = param_2 | *(ushort *)(param_1 + 0x14);
  }
  *(ushort *)(param_1 + 0x14) = param_2;
  return;
}

