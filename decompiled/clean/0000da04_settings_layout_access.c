/* Semantic review: high. Packed word bits 10 and 11; selects alternate layout.
 * Address 0x0000da04; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000da04; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Packed word bits 10 and 11; selects alternate layout */

uint settings_layout_access(uint write_flag,short value)

{
  gp = &__global_pointer;
  if (write_flag == 0) {
    return (int)(uint)settings_word >> 10 & 3;
  }
  settings_word = settings_word & 0xf3ff | value << 10;
  return 0xfffff3ff;
}

