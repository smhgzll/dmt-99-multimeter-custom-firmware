/* Original address: 0x0000d38a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: PA4 CS PA5 SCK PA6 MISO PA7 MOSI and SPI1 configuration */

void spi1_flash_init(void)

{
  GPIO_Init_Recovered GStack_40;
  SPI_Init_Recovered SStack_34;
  
  gp = &__global_pointer;
  GStack_40.pin = 0;
  GStack_40.pad = 0;
  GStack_40.speed = 0;
  GStack_40.mode = 0;
  SStack_34.direction = 0;
  SStack_34.mode = 0;
  SStack_34.data_size = 0;
  SStack_34.clock_polarity = 0;
  SStack_34.clock_phase = 0;
  SStack_34.nss = 0;
  SStack_34.prescaler = 0;
  SStack_34.first_bit = 0;
  SStack_34.crc_polynomial = 0;
  RCC_APB2PeriphClockCmd(0x1004,1);
  GStack_40.pin = 0x10;
  GStack_40.mode = 0x10;
  GStack_40.speed = 3;
  GPIO_Init(&GPIOA,&GStack_40);
  GPIO_SetBits(&GPIOA,0x10);
  GStack_40.pin = 0x20;
  GStack_40.mode = 0x18;
  GStack_40.speed = 3;
  GPIO_Init(&GPIOA,&GStack_40);
  GStack_40.pin = 0x40;
  GStack_40.mode = 4;
  GPIO_Init(&GPIOA,&GStack_40);
  GStack_40.pin = 0x80;
  GStack_40.mode = 0x18;
  GStack_40.speed = 3;
  GPIO_Init(&GPIOA,&GStack_40);
  SStack_34.direction = 0;
  SStack_34.mode = 0x104;
  SStack_34.data_size = 0;
  SStack_34.clock_polarity = 2;
  SStack_34.clock_phase = 1;
  SStack_34.nss = 0x200;
  SStack_34.crc_polynomial = 7;
  SStack_34.prescaler = 0x10;
  SStack_34.first_bit = 0;
  SPI_Init(&SPI1,&SStack_34);
  SPI_Cmd(&SPI1,1);
  return;
}

