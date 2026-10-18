/* Semantic review: high. Internal flash busy and error status logic.
 * Address 0x0000c9b2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c9b2; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Internal flash busy and error status logic */

char FLASH_GetStatus(void)

{
  uint uVar1;
  char cVar2;
  
  gp = &__global_pointer;
  uVar1 = DAT_ram_4002200c;
  cVar2 = '\x01';
  if ((uVar1 & 1) == 0) {
    uVar1 = DAT_ram_4002200c;
    cVar2 = ((uVar1 & 0x10) == 0) + '\x03';
  }
  return cVar2;
}

