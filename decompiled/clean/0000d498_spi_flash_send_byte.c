/* Semantic review: high. Wrapper around SPI1 exchange.
 * Address 0x0000d498; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d498; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Wrapper around SPI1 exchange */

void spi_flash_send_byte(uchar data)

{
  gp = &__global_pointer;
  spi1_exchange_byte(data);
  return;
}

