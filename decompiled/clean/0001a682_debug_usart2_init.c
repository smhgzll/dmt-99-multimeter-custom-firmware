/* Semantic review: high. PA2 alternate output USART2 at main argument 115200.
 * Address 0x0001a682; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001a682; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: PA2 alternate output USART2 at main argument 115200 */

void debug_usart2_init(uint32_t param_1)

{
  GPIO_Init_Recovered GStack_2c;
  USART_Init_Recovered UStack_20;
  
  gp = &__global_pointer;
  RCC_APB1PeriphClockCmd(0x20000,1);
  RCC_APB2PeriphClockCmd(4,1);
  GStack_2c.pin = 4;
  GStack_2c.speed = 3;
  GStack_2c.mode = 0x18;
  GPIO_Init(&GPIOA,&GStack_2c);
  UStack_20.parity = 0;
  UStack_20.mode = 8;
  UStack_20.word_length = 0;
  UStack_20.stop_bits = 0;
  UStack_20.flow_control = 0;
  UStack_20.baud = param_1;
  USART_Init(&USART2,&UStack_20);
  USART_Cmd(&USART2,1);
  return;
}

