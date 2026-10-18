/* Original address: 0x00007562; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: GPIOA pin 8 floating input */

void charge_detect_gpio_init(void)

{
  GPIO_Init_Recovered GStack_1c;
  
  gp = &__global_pointer;
  GStack_1c.pin = 0;
  GStack_1c.pad = 0;
  GStack_1c.speed = 0;
  GStack_1c.mode = 0;
  RCC_APB2PeriphClockCmd(4,1);
  GStack_1c.pin = 0x100;
  GStack_1c.mode = 4;
  GStack_1c.speed = 3;
  GPIO_Init(&GPIOA,&GStack_1c);
  return;
}

