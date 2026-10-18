/* Original address: 0x0000ce0c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI status flag mask */

bool SPI_I2S_GetFlagStatus(int param_1,ushort param_2)

{
  gp = &__global_pointer;
  return (*(ushort *)(param_1 + 8) & param_2) != 0;
}

