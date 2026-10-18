/* Original address: 0x0000c468; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 53; IDLE clears and rearms DMA plus 13-byte event snapshot */

void USART1_IRQHandler(void)

{
  int iVar1;
  int iVar2;
  uint8_t *puVar3;
  
  gp = &__global_pointer;
  iVar2 = USART_GetITStatus(&USART1,0x424);
  if (iVar2 != 0) {
    DMA_Cmd(&DMA1_Channel5,0);
    USART_ClearFlag(&USART1,0x10);
    USART_ReceiveData(&USART1);
    measurement_rx_suppressed = 0;
    DMA_SetCurrDataCounter(&DMA1_Channel5,0xd);
    DMA_Cmd(&DMA1_Channel5,1);
    if (((((measurement_dma_frame.status_or_segments0 & 0xe0) == 0 &&
           (measurement_dma_frame.packed_segments[0] & 0xf) == 0) &&
         ((measurement_dma_frame.packed_segments[0] & 0xe0) == 0 &&
          (measurement_dma_frame.packed_segments[1] & 0xf) == 0)) &&
        ((measurement_dma_frame.packed_segments[1] & 0xe0) == 0 &&
         (measurement_dma_frame.packed_segments[2] & 0xf) == 0)) &&
       ((measurement_dma_frame.packed_segments[2] & 0xe0) == 0 &&
        (measurement_dma_frame.packed_segments[3] & 0xf) == 0)) {
      measurement_rx_suppressed = 1;
    }
    if (measurement_dma_frame.status_or_segments0 == 0xfd) {
      iVar2 = 0;
      do {
        iVar1 = iVar2 + -3;
        puVar3 = &measurement_saved_event + iVar2;
        iVar2 = iVar2 + 1;
        *puVar3 = measurement_dma_frame.packed_segments[iVar1];
      } while (iVar2 != 0xd);
      measurement_event_consumed = 0;
    }
  }
  return;
}

