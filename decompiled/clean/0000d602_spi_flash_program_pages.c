/* Semantic review: high. Splits writes at 256-byte page boundaries.
 * Address 0x0000d602; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d602; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Splits writes at 256-byte page boundaries */

void spi_flash_program_pages(uchar *source,uint address,ushort count)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = (uint)count;
  gp = &__global_pointer;
  uVar3 = 0x100 - (address & 0xff);
  if (uVar2 < (uVar3 & 0xffff)) {
    uVar3 = uVar2;
  }
  while( true ) {
    uVar1 = uVar3 & 0xffff;
    spi_flash_page_program(source,address,(ushort)uVar3);
    if (uVar2 == uVar1) break;
    uVar2 = uVar2 - uVar1 & 0xffff;
    source = source + uVar1;
    address = address + uVar1;
    uVar3 = uVar2;
    if (0x100 < uVar2) {
      uVar3 = 0x100;
    }
  }
  return;
}

