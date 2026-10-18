/* Original address: 0x0000d9e6; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Packed word bits 3 and 4; UI meaning not yet established */

uint settings_field_3_4_access(uint write_flag,short value)

{
  gp = &__global_pointer;
  if (write_flag == 0) {
    return (int)(uint)settings_word >> 3 & 3;
  }
  settings_word = settings_word & 0xffe7 | value << 3;
  return write_flag;
}

