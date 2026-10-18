/* Semantic review: high. Shared restore fallthroughs at 0x6640 0x664a 0x6654.
 * Address 0x0000663c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000663c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Shared restore fallthroughs at 0x6640 0x664a 0x6654 */

void wch_restore_12(void)

{
  gp = &__global_pointer;
  return;
}

