/* Original address: 0x0000da28; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Packed word bit 5; temperature display call site */

uint settings_temperature_access(uint write_flag,short value)

{
  gp = &__global_pointer;
  if (write_flag == 0) {
    return (int)(uint)settings_word >> 5 & 1;
  }
  settings_word = settings_word & 0xff9f | value << 5;
  return write_flag;
}

