/* Original address: 0x0000dcd0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Sector index shifted by 12 then preserved-sector SPI write */

undefined4 disk_write(uint param_1,uchar *param_2,int param_3,int param_4)

{
  gp = &__global_pointer;
  if ((param_4 != 0) && (param_1 < 2)) {
    spi_flash_write_preserve_sector
              (param_2,param_3 << 0xc,(ushort)((uint)(param_4 << 0x1c) >> 0x10));
    return 0;
  }
  return 4;
}

