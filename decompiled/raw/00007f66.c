/* Original address: 0x00007f66; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: TIM8 PWM on PC6; duty used on display startup and power-off */

void tim8_backlight_pwm_config(ushort param_1,ushort param_2,undefined2 param_3)

{
  uint uStack_38;
  uint uStack_34;
  undefined2 uStack_30;
  GPIO_Init_Recovered GStack_2c;
  undefined *puStack_20;
  undefined4 uStack_1c;
  uint uStack_18;
  undefined4 uStack_14;
  
  gp = &__global_pointer;
  GStack_2c.pin = 0;
  GStack_2c.pad = 0;
  GStack_2c.speed = 0;
  GStack_2c.mode = 0;
  puStack_20 = (undefined *)0x0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  uStack_38 = 0;
  uStack_34 = 0;
  uStack_30 = 0;
  RCC_APB2PeriphClockCmd(0x2000,1);
  RCC_APB2PeriphClockCmd(0x10,1);
  GStack_2c.pin = 0x40;
  GStack_2c.mode = 0x18;
  GStack_2c.speed = 3;
  GPIO_Init(&GPIOC,&GStack_2c);
  uStack_34 = (uint)param_1;
  uStack_38 = (uint)param_2;
  TIM_TimeBaseInit(&TIM8,&uStack_38);
  puStack_20 = &DAT_ram_00010070;
  uStack_1c = CONCAT22(param_3,(undefined2)uStack_1c);
  uStack_18 = uStack_18 & 0xffff0000;
  TIM_OC1Init(&TIM8,&puStack_20);
  TIM_CtrlPWMOutputs(&TIM8,1);
  TIM_OC1PreloadConfig(&TIM8,8);
  TIM_ARRPreloadConfig(&TIM8,1);
  TIM_Cmd(&TIM8,1);
  return;
}

