/* Semantic review: high. 0x9f and three response bytes with PA4 CS.
 * Address 0x0000d4b4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d4b4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x9f and three response bytes with PA4 CS */

uint spi_flash_read_jedec_id(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  
  gp = &__global_pointer;
  GPIOA.BCR = 0x10;
  spi_flash_send_byte(0x9f);
  bVar1 = spi1_exchange_byte(0xff);
  bVar2 = spi1_exchange_byte(0xff);
  bVar3 = spi1_exchange_byte(0xff);
  GPIOA.BSHR = 0x10;
  return (uint)bVar3 | (uint)bVar1 << 0x10 | (uint)bVar2 << 8;
}

