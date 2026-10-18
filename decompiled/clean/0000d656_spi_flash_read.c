/* Semantic review: high. 0x03 command 24-bit address and dummy-clock reads.
 * Address 0x0000d656; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d656; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0x03 command 24-bit address and dummy-clock reads */

void spi_flash_read(uchar *destination,uint address,ushort count)

{
  uint uVar1;
  uchar uVar2;
  
  gp = &__global_pointer;
  GPIOA.BCR = 0x10;
  spi1_exchange_byte('\x03');
  spi1_exchange_byte((uchar)(address >> 0x10));
  spi1_exchange_byte((uchar)(address >> 8));
  spi1_exchange_byte((uchar)address);
  for (uVar1 = 0; (uVar1 & 0xffff) < (uint)count; uVar1 = uVar1 + 1) {
    uVar2 = spi1_exchange_byte(0xff);
    destination[uVar1] = uVar2;
  }
  GPIOA.BSHR = 0x10;
  return;
}

