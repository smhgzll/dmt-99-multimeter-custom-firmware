/* Semantic review: high. SPI DATAR halfword store.
 * Address 0x0000ce04; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ce04; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI DATAR halfword store */

void SPI_I2S_SendData(int param_1,undefined2 param_2)

{
  gp = &__global_pointer;
  *(undefined2 *)(param_1 + 0xc) = param_2;
  return;
}

