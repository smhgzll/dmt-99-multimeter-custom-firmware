/* Semantic review: high. Fast-mode lock bit.
 * Address 0x0000ca44; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ca44; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Fast-mode lock bit */

void FLASH_Lock_Fast(void)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = DAT_ram_40022010;
  DAT_ram_40022010 = uVar1 | 0x8000;
  return;
}

