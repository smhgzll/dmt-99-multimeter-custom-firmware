/* Original address: 0x0000d1b0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART DATAR write masked to nine bits */

void USART_SendData(USART_Registers *usart,ushort data)

{
  gp = &__global_pointer;
  usart->DATAR = data & 0x1ff;
  return;
}

