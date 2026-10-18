/* Original address: 0x0000d4f8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x06 command under PA4 CS */

void spi_flash_write_enable(void)

{
  gp = &__global_pointer;
  GPIOA.BCR = 0x10;
  spi_flash_send_byte('\x06');
  GPIOA.BSHR = 0x10;
  return;
}

