/* Original address: 0x0000d1c0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART STATR mask */

uint USART_GetFlagStatus(USART_Registers *usart,ushort flags)

{
  gp = &__global_pointer;
  return (uint)((usart->STATR & flags) != 0);
}

