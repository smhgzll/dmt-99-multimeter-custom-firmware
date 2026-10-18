/* Semantic review: high. Internal flash CTLR lock bit.
 * Address 0x0000c99c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c99c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Internal flash CTLR lock bit */

void FLASH_Lock(void)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = DAT_ram_40022010;
  DAT_ram_40022010 = uVar1 | 0x80;
  return;
}

