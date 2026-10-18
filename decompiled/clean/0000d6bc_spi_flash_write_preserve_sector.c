/* Semantic review: high. 4096-byte read modify erase rewrite sequence.
 * Address 0x0000d6bc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d6bc; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */
/* high: 4096-byte read modify erase rewrite sequence */

void spi_flash_write_preserve_sector(uchar *source,uint address,ushort count)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uchar *source_00;
  ushort count_00;
  uint uVar4;
  uint uVar5;
  uint sector_index;
  uint address_00;
  
  uVar3 = (uint)count;
  gp = &__global_pointer;
  uVar5 = address & 0xfff;
  sector_index = address >> 0xc;
  uVar2 = 0x1000 - uVar5;
  if (uVar3 < (0x1000 - uVar5 & 0xffff)) {
    uVar2 = uVar3;
  }
  do {
    uVar1 = uVar2 & 0xffff;
    address_00 = sector_index << 0xc;
    spi_flash_read(&flash_sector_scratch,address_00,0x1000);
    uVar4 = 0;
    do {
      if (uVar1 <= (uVar4 & 0xffff)) {
        count_00 = (ushort)uVar2;
        source_00 = source;
        address_00 = address;
        goto LAB_ram_0000d754;
      }
      uVar4 = uVar4 + 1;
    } while (*(char *)(uVar5 + uVar4 + 0x200002cb) == -1);
    spi_flash_erase_sector(sector_index);
    for (uVar2 = 0; (uVar2 & 0xffff) < uVar1; uVar2 = uVar2 + 1) {
      (&flash_sector_scratch)[uVar5 + uVar2] = source[uVar2];
    }
    count_00 = 0x1000;
    source_00 = &flash_sector_scratch;
LAB_ram_0000d754:
    spi_flash_program_pages(source_00,address_00,count_00);
    if (uVar3 == uVar1) {
      return;
    }
    source = source + uVar1;
    address = address + uVar1;
    uVar3 = uVar3 - uVar1 & 0xffff;
    sector_index = sector_index + 1;
    uVar5 = 0;
    uVar2 = uVar3;
    if (0x1000 < uVar3) {
      uVar2 = 0x1000;
    }
  } while( true );
}

