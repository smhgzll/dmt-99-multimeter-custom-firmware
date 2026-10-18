/* Semantic review: high. PC2 8 9 10 11 12 pullups and falling-edge EXTI mapping.
 * Address 0x00007944; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007944; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: PC2 8 9 10 11 12 pullups and falling-edge EXTI mapping */

void buttons_gpio_exti_init(void)

{
  undefined4 uStack_44;
  undefined4 uStack_40;
  GPIO_Init_Recovered GStack_3c;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  gp = &__global_pointer;
  GStack_3c.pin = 0;
  GStack_3c.pad = 0;
  GStack_3c.speed = 0;
  GStack_3c.mode = 0;
  uStack_30 = 0;
  uStack_2c = 0;
  uStack_28 = 0;
  uStack_24 = 0;
  uStack_44 = 0;
  uStack_40 = 0;
  RCC_APB2PeriphClockCmd(0x11,1);
  RCC_APB2PeriphClockCmd(0x10,1);
  GStack_3c.mode = 0x10;
  GStack_3c.speed = 3;
  GStack_3c.pin = 2;
  GPIO_Init(&GPIOC,&GStack_3c);
  delay_ms(300);
  GPIO_WriteBit(&GPIOC,2,1);
  GStack_3c.pin = 0x400;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,10);
  uStack_30 = 0x400;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44._0_3_ = 0x10138;
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  GStack_3c.pin = 0x800;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,0xb);
  uStack_30 = 0x800;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44._0_3_ = 0x10138;
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  GStack_3c.pin = 0x200;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,9);
  uStack_30 = 0x200;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44._0_3_ = 0x10127;
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  GStack_3c.pin = 0x1000;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,0xc);
  uStack_30 = 0x1000;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44._0_3_ = 0x10138;
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  GStack_3c.pin = 0x100;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,8);
  uStack_30 = 0x100;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44._0_3_ = 0x20127;
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  GStack_3c.pin = 4;
  GStack_3c.mode = 0x48;
  GPIO_Init(&GPIOC,&GStack_3c);
  GPIO_EXTILineConfig(2,2);
  uStack_30 = 4;
  uStack_28 = 0xc;
  uStack_24 = 1;
  uStack_2c = 0;
  EXTI_Init(&uStack_30);
  uStack_44 = CONCAT13(uStack_44._3_1_,0x10118);
  uStack_40 = 1;
  NVIC_Init(&uStack_44);
  return;
}

