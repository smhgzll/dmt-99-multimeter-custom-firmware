/* Semantic review: high. Packed word bits 8 and 9.
 * Address 0x0000d944; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d944; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Packed word bits 8 and 9 */

uint settings_theme_access(uint write_flag,short value)

{
  gp = &__global_pointer;
  if (write_flag == 0) {
    return (int)(uint)settings_word >> 8 & 3;
  }
  settings_word = settings_word & 0xfcff | value << 8;
  return write_flag;
}

