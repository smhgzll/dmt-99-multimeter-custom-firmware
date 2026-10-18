/* Original address: 0x0000d782; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000d782(void)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  DAT_ram_20000210 = spi_flash_read_jedec_id();
  DAT_ram_2000021a = 0;
  if (DAT_ram_20000210 == 0xef4016) {
LAB_ram_0000d85a:
    uVar2 = 0x400000;
  }
  else {
    if (DAT_ram_20000210 < 0xef4017) {
      if (DAT_ram_20000210 == 0xef3013) {
        uVar2 = 0x80000;
        goto LAB_ram_0000d842;
      }
      if (DAT_ram_20000210 < 0xef3014) {
        if (DAT_ram_20000210 == 0xef3011) {
          uVar2 = 0x20000;
          goto LAB_ram_0000d842;
        }
        uVar1 = 0xef3012;
        uVar2 = 0x40000;
        goto LAB_ram_0000d7e0;
      }
      if (DAT_ram_20000210 == 0xef4014) {
        uVar2 = 0x100000;
        goto LAB_ram_0000d842;
      }
      if (DAT_ram_20000210 < 0xef4015) {
        uVar1 = 0xef3015;
        uVar2 = 0x200000;
        goto LAB_ram_0000d7e0;
      }
    }
    else {
      if (DAT_ram_20000210 == 0xef6016) goto LAB_ram_0000d85a;
      if (DAT_ram_20000210 < 0xef6017) {
        if (DAT_ram_20000210 == 0xef4018) {
LAB_ram_0000d860:
          uVar2 = 0x1000000;
          goto LAB_ram_0000d842;
        }
        if (DAT_ram_20000210 < 0xef4018) {
LAB_ram_0000d866:
          uVar2 = 0x800000;
          goto LAB_ram_0000d842;
        }
        uVar1 = 0xef4019;
      }
      else {
        if (DAT_ram_20000210 == 0xef6018) goto LAB_ram_0000d860;
        if (DAT_ram_20000210 < 0xef6018) goto LAB_ram_0000d866;
        uVar1 = 0xef6019;
      }
      uVar2 = 0x2000000;
LAB_ram_0000d7e0:
      if (DAT_ram_20000210 == uVar1) goto LAB_ram_0000d842;
      if (DAT_ram_20000210 == 0xffffffff) {
        gp = &__global_pointer;
        DAT_ram_20000214 = 0;
        DAT_ram_20000218 = 0;
        DAT_ram_2000021a = 0;
        return;
      }
      if (DAT_ram_20000210 == 0) {
        gp = &__global_pointer;
        DAT_ram_20000214 = 0;
        DAT_ram_20000218 = 0;
        DAT_ram_2000021a = 0;
        return;
      }
    }
    uVar2 = 0x200000;
  }
LAB_ram_0000d842:
  DAT_ram_20000218 = 0x1000;
  DAT_ram_20000214 = uVar2 >> 0xc;
  return;
}

