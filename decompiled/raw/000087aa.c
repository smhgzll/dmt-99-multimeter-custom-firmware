/* Original address: 0x000087aa; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: GPIOB data port PC control pins reset and LCD initialization commands */

void display_init(void)

{
  GPIO_Init_Recovered GStack_1c;
  
  gp = &__global_pointer;
  RCC_APB2PeriphClockCmd(0x1c,1);
  GStack_1c.pin = 0x20b8;
  GStack_1c.mode = 0x10;
  GStack_1c.speed = 3;
  GPIO_Init(&GPIOC,&GStack_1c);
  GPIO_SetBits(&GPIOC,0x20b8);
  GStack_1c.speed = 3;
  GStack_1c.pin = 0xffff;
  GStack_1c.mode = 0x10;
  GPIO_Init(&GPIOB,&GStack_1c);
  GPIO_SetBits(&GPIOB,0xffff);
  GPIO_WriteBit(&GPIOC,8,1);
  delay_ms(10);
  GPIO_WriteBit(&GPIOC,8,0);
  delay_ms(0x14);
  GPIO_WriteBit(&GPIOC,8,1);
  delay_ms(0x14);
  display_write_command(0x11);
  delay_ms(0x78);
  display_write_command(0x36);
  display_write_data(0);
  display_write_command(0x3a);
  display_write_data(5);
  display_write_command(0xb2);
  display_write_data(0xc);
  display_write_data(0xc);
  display_write_data(0);
  display_write_data(0x33);
  display_write_data(0x33);
  display_write_command(0xb7);
  display_write_data(0x35);
  display_write_command(0xbb);
  display_write_data(0x28);
  display_write_command(0xc0);
  display_write_data(0x2c);
  display_write_command(0xc2);
  display_write_data(1);
  display_write_command(0xc3);
  display_write_data(0xb);
  display_write_command(0xc4);
  display_write_data(0x20);
  display_write_command(0xc6);
  display_write_data(0xf);
  display_write_command(0xd0);
  display_write_data(0xa4);
  display_write_data(0xa1);
  display_write_command(0xe0);
  display_write_data(0xd0);
  display_write_data(1);
  display_write_data(8);
  display_write_data(0xf);
  display_write_data(0x11);
  display_write_data(0x2a);
  display_write_data(0x36);
  display_write_data(0x55);
  display_write_data(0x44);
  display_write_data(0x3a);
  display_write_data(0xb);
  display_write_data(6);
  display_write_data(0x11);
  display_write_data(0x20);
  display_write_command(0xe1);
  display_write_data(0xd0);
  display_write_data(2);
  display_write_data(7);
  display_write_data(10);
  display_write_data(0xb);
  display_write_data(0x18);
  display_write_data(0x34);
  display_write_data(0x43);
  display_write_data(0x4a);
  display_write_data(0x2b);
  display_write_data(0x1b);
  display_write_data(0x1c);
  display_write_data(0x22);
  display_write_data(0x1f);
  display_write_command(0x29);
  display_write_command(0x2c);
  display_set_orientation(1);
  display_fill(0);
  return;
}

