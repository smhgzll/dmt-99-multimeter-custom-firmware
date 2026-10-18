/* Semantic review: high. Wrapper exchanges dummy byte.
 * Address 0x0000d4a4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d4a4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Wrapper exchanges dummy byte */

uchar spi_flash_receive_byte(void)

{
  uchar uVar1;
  
  gp = &__global_pointer;
  uVar1 = spi1_exchange_byte(0xff);
  return uVar1;
}

