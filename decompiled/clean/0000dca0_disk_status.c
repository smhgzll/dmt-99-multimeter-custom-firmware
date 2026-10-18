/* Semantic review: medium. FatFs-facing drive status wrapper returns zero.
 * Address 0x0000dca0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000dca0; Ghidra pseudocode, not compilable reconstructed source. */

/* medium: FatFs-facing drive status wrapper returns zero */

undefined4 disk_status(void)

{
  gp = &__global_pointer;
  return 0;
}

