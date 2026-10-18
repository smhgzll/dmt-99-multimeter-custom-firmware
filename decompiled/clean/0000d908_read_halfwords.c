/* Semantic review: high. 16-bit source-to-destination copy.
 * Address 0x0000d908; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d908; Ghidra pseudocode, not compilable reconstructed source. */

/* high: 16-bit source-to-destination copy */

void read_halfwords(ushort *source,ushort *destination,ushort count)

{
  uint uVar1;
  
  gp = &__global_pointer;
  for (uVar1 = 0; (uVar1 & 0xffff) < (uint)count; uVar1 = uVar1 + 1) {
    destination[uVar1] = source[uVar1];
  }
  return;
}

