/* Semantic review: high. Mount calls and BOOT resource handling.
 * Address 0x0000dd4e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000dd4e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Mount calls and BOOT resource handling */

void filesystem_resources_init(void)

{
  gp = &__global_pointer;
  spi_flash_init();
  FUN_ram_0000fc66(0,&DAT_ram_20000144,&DAT_ram_20002304);
  DAT_ram_20000230 = FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023cc0,1);
  if (DAT_ram_20000230 == 0) {
    FUN_ram_0000fb48("0:BOOT");
  }
  DAT_ram_20000230 = FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023ccc,1);
  return;
}

