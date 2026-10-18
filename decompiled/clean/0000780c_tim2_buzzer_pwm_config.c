/* Semantic review: medium. TIM2 PWM on PA1; disable counterpart drives pin low.
 * Address 0x0000780c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000780c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: TIM2 PWM on PA1; disable counterpart drives pin low */

void tim2_buzzer_pwm_config(ushort param_1,ushort param_2,undefined2 param_3)

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
  RCC_APB1PeriphClockCmd(1,1);
  RCC_APB2PeriphClockCmd(4,1);
  GStack_2c.pin = 2;
  GStack_2c.mode = 0x18;
  GStack_2c.speed = 3;
  GPIO_Init(&GPIOA,&GStack_2c);
  uStack_34 = (uint)param_1;
  uStack_38 = (uint)param_2;
  TIM_TimeBaseInit(0x40000000,&uStack_38);
  puStack_20 = &DAT_ram_00010070;
  uStack_1c = CONCAT22(param_3,(undefined2)uStack_1c);
  uStack_18 = uStack_18 & 0xffff0000;
  TIM_OC2Init(0x40000000,&puStack_20);
  TIM_CtrlPWMOutputs(0x40000000,1);
  TIM_OC2PreloadConfig(0x40000000,8);
  TIM_ARRPreloadConfig(0x40000000,1);
  TIM_Cmd(0x40000000,1);
  return;
}

