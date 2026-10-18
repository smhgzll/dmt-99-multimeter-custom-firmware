/* Semantic review: high. USART STATR complement-mask write.
 * Address 0x0000d1ca; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d1ca; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART STATR complement-mask write */

void USART_ClearFlag(USART_Registers *usart,ushort flags)

{
  gp = &__global_pointer;
  usart->STATR = ~flags;
  return;
}

