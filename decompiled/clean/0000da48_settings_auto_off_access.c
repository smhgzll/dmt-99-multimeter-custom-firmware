/* Semantic review: high. Packed word bits 6 and 7; 0 300 900 1800 second thresholds.
 * Address 0x0000da48; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000da48; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Packed word bits 6 and 7; 0 300 900 1800 second thresholds */

uint settings_auto_off_access(uint write_flag,int value)

{
  gp = &__global_pointer;
  if (write_flag == 0) {
    return (int)(uint)settings_word >> 6 & 3;
  }
  settings_word = settings_word & 0xff3f | (ushort)(value << 6);
  if (value == 0) {
    auto_off_seconds = (undefined *)0x0;
    return write_flag;
  }
  if (value == 1) {
    auto_off_seconds = (undefined *)0x12c;
  }
  else {
    if (value != 2) {
      if (value == 3) {
        auto_off_seconds = (undefined *)0x708;
      }
      return write_flag;
    }
    auto_off_seconds = &mdbase;
  }
  return write_flag;
}

