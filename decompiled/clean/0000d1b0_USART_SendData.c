/* Semantic review: high. USART DATAR write masked to nine bits.
 * Address 0x0000d1b0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d1b0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART DATAR write masked to nine bits */

void USART_SendData(USART_Registers *usart,ushort data)

{
  gp = &__global_pointer;
  usart->DATAR = data & 0x1ff;
  return;
}

