/* Original address: 0x0001a626; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Core-clock-derived millisecond and microsecond factors */

void delay_init(void)

{
  gp = &__global_pointer;
  DAT_ram_20000264 = (char)(system_core_clock_hz / 8000000);
  DAT_ram_20000262 = ((ushort)(system_core_clock_hz / 8000000) & 0xff) * 1000;
  return;
}

