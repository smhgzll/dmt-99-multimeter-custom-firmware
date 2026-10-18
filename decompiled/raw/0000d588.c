/* Original address: 0x0000d588; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x02 command 24-bit address and up to 256 data bytes */

void spi_flash_page_program(uchar *source,uint address,ushort count)

{
  uchar *puVar1;
  byte bVar2;
  uint uVar3;
  
  uVar3 = (uint)count;
  gp = &__global_pointer;
  spi_flash_write_enable();
  GPIOA.BCR = 0x10;
  spi_flash_send_byte('\x02');
  spi_flash_send_byte((uchar)(address >> 0x10));
  spi_flash_send_byte((uchar)(address >> 8));
  spi_flash_send_byte((uchar)address);
  if (0x100 < uVar3) {
    uVar3 = 0x100;
  }
  puVar1 = source + uVar3;
  for (; source != puVar1; source = source + 1) {
    spi_flash_send_byte(*source);
  }
  GPIOA.BSHR = 0x10;
  do {
    bVar2 = spi_flash_read_status();
  } while ((bVar2 & 1) != 0);
  return;
}

