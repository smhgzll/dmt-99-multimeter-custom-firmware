/* Semantic review: high. 0x20 command and three address bytes with sector index shift.
 * Address 0x0000d538; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d538; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x20 command and three address bytes with sector index shift */

void spi_flash_erase_sector(uint sector_index)

{
  byte bVar1;
  
  gp = &__global_pointer;
  spi_flash_write_enable();
  GPIOA.BCR = 0x10;
  spi_flash_send_byte(' ');
  spi_flash_send_byte((uchar)((sector_index << 0xc) >> 0x10));
  spi_flash_send_byte((uchar)((sector_index << 0xc) >> 8));
  spi_flash_send_byte('\0');
  GPIOA.BSHR = 0x10;
  do {
    bVar1 = spi_flash_read_status();
  } while ((bVar1 & 1) != 0);
  return;
}

