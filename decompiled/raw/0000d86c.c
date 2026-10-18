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

