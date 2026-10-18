/* Semantic review: high. USART1 PA9 PA10 13-byte circular DMA1 channel 5.
 * Address 0x0000c350; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c350; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: USART1 PA9 PA10 13-byte circular DMA1 channel 5 */

void measurement_uart_dma_init(uint baud)

{
  undefined2 uStack_60;
  undefined1 uStack_5e;
  undefined1 uStack_5d;
  undefined4 uStack_5c;
  GPIO_Init_Recovered GStack_58;
  USART_Init_Recovered UStack_4c;
  DMA_Init_Recovered DStack_3c;
  
  gp = &__global_pointer;
  GStack_58.pin = 0;
  GStack_58.pad = 0;
  GStack_58.speed = 0;
  GStack_58.mode = 0;
  UStack_4c.baud = 0;
  UStack_4c.word_length = 0;
  UStack_4c.stop_bits = 0;
  UStack_4c.parity = 0;
  UStack_4c.mode = 0;
  UStack_4c.flow_control = 0;
  UStack_4c.pad = 0;
  RCC_APB2PeriphClockCmd(0x4004,1);
  GStack_58.pin = 0x200;
  GStack_58.speed = 3;
  GStack_58.mode = 0x18;
  GPIO_Init(&GPIOA,&GStack_58);
  GStack_58.pin = 0x400;
  GStack_58.mode = 4;
  GPIO_Init(&GPIOA,&GStack_58);
  UStack_4c.parity = 0;
  UStack_4c.mode = 0xc;
  UStack_4c.word_length = 0;
  UStack_4c.stop_bits = 0;
  UStack_4c.baud = baud;
  USART_Init(&USART1,&UStack_4c);
  uStack_60 = 0x235;
  uStack_5e = 2;
  uStack_5d = 0;
  uStack_5c = 1;
  NVIC_Init(&uStack_60);
  USART_ITConfig(&USART1,0x424,1);
  USART_ITConfig(&USART1,0x525,1);
  USART_Cmd(&USART1,1);
  RCC_AHBPeriphClockCmd(1,1);
  DStack_3c.peripheral_address = 0x40013804;
  DStack_3c.memory_address = (uint32_t)&measurement_dma_frame;
  DStack_3c.count = 0xd;
  DStack_3c.memory_increment = 0x80;
  DStack_3c.circular = 0x20;
  DStack_3c.priority = 0x2000;
  DStack_3c.direction = 0;
  DStack_3c.peripheral_increment = 0;
  DStack_3c.peripheral_width = 0;
  DStack_3c.memory_width = 0;
  DStack_3c.memory_to_memory = 0;
  DMA_Init(&DMA1_Channel5,&DStack_3c);
  uStack_60 = 0x11f;
  _uStack_5e = CONCAT11(uStack_5d,1);
  uStack_5c = 1;
  NVIC_Init(&uStack_60);
  DMA_ITConfig(&DMA1_Channel5,2,1);
  DMA_Cmd(&DMA1_Channel5,1);
  USART_DMACmd(&USART1,0x40,1);
  return;
}

