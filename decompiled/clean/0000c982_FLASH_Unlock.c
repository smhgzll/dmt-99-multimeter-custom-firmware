/* Semantic review: high. Internal flash two-key unlock sequence.
 * Address 0x0000c982; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c982; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Internal flash two-key unlock sequence */

void FLASH_Unlock(void)

{
  gp = &__global_pointer;
  DAT_ram_40022004 = 0x45670123;
  DAT_ram_40022004 = 0xcdef89ab;
  return;
}

