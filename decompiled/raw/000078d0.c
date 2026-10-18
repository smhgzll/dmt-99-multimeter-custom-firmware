/* Original address: 0x000078d0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: TIM2 compare output and PA1 level control */

void buzzer_enable(int param_1)

{
  GPIO_Init_Recovered GStack_1c;
  
  gp = &__global_pointer;
  if (param_1 == 1) {
    FUN_ram_000078ba();
  }
  else {
    TIM_SetCompare2(0x40000000,0);
    GStack_1c.pin = 2;
    GStack_1c.mode = 0x10;
    GStack_1c.pad = 0;
    GStack_1c.speed = 3;
    GPIO_Init(&GPIOA,&GStack_1c);
    GPIO_ResetBits(&GPIOA,2);
  }
  return;
}

