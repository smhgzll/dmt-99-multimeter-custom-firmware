/* Original address: 0x0000d1b8; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART DATAR read masked to nine bits */

ushort USART_ReceiveData(USART_Registers *usart)

{
  gp = &__global_pointer;
  return usart->DATAR & 0x1ff;
}

