/* Semantic review: high. Fast erase with address rounded to 256-byte boundary.
 * Address 0x0000ca52; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ca52; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Fast erase with address rounded to 256-byte boundary */

void FLASH_ErasePage_Fast(uint param_1)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = DAT_ram_40022010;
  DAT_ram_40022010 = uVar1 | 0x20000;
  DAT_ram_40022014 = param_1 & 0xffffff00;
  uVar1 = DAT_ram_40022010;
  DAT_ram_40022010 = uVar1 | 0x40;
  do {
    uVar1 = DAT_ram_4002200c;
  } while ((uVar1 & 1) != 0);
  uVar1 = DAT_ram_40022010;
  DAT_ram_40022010 = uVar1 & 0xfffdffff;
  return;
}

