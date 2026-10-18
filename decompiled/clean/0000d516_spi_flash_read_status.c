/* Semantic review: high. 0x05 command under PA4 CS.
 * Address 0x0000d516; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d516; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x05 command under PA4 CS */

uchar spi_flash_read_status(void)

{
  uchar uVar1;
  
  gp = &__global_pointer;
  GPIOA.BCR = 0x10;
  spi_flash_send_byte('\x05');
  uVar1 = spi_flash_receive_byte();
  GPIOA.BSHR = 0x10;
  return uVar1;
}

