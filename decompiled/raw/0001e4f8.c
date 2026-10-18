/* Original address: 0x0001e4f8; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001e4f8(uint param_1)

{
  gp = &__global_pointer;
  if (0xffff < param_1) {
    if (0xffffff < param_1) {
      return 8 - (uint)(byte)(&DAT_ram_000270d0)[param_1 >> 0x18];
    }
    return 0x10 - (uint)(byte)(&DAT_ram_000270d0)[param_1 >> 0x10];
  }
  if (param_1 < 0x100) {
    return 0x20 - (uint)(byte)(&DAT_ram_000270d0)[param_1];
  }
  return 0x18 - (uint)(byte)(&DAT_ram_000270d0)[param_1 >> 8];
}

