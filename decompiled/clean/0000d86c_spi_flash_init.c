/* Semantic review: high. Calls SPI1 initialization and JEDEC ID read.
 * Address 0x0000d86c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d86c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Calls SPI1 initialization and JEDEC ID read */

void spi_flash_init(void)

{
  gp = &__global_pointer;
  spi1_flash_init();
  FUN_ram_0000d782();
  return;
}

