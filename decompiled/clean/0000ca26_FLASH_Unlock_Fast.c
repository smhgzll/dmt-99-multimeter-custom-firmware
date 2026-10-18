/* Semantic review: high. Fast-mode two-key unlock.
 * Address 0x0000ca26; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ca26; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Fast-mode two-key unlock */

void FLASH_Unlock_Fast(void)

{
  gp = &__global_pointer;
  DAT_ram_40022004 = 0x45670123;
  DAT_ram_40022004 = 0xcdef89ab;
  DAT_ram_40022024 = 0x45670123;
  DAT_ram_40022024 = 0xcdef89ab;
  return;
}

