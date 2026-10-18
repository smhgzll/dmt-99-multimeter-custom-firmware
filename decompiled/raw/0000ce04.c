/* Original address: 0x0000ce04; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI DATAR halfword store */

void SPI_I2S_SendData(int param_1,undefined2 param_2)

{
  gp = &__global_pointer;
  *(undefined2 *)(param_1 + 0xc) = param_2;
  return;
}

