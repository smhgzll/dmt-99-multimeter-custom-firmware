/* Semantic review: high. SPI DATAR halfword load.
 * Address 0x0000ce08; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ce08; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI DATAR halfword load */

undefined2 SPI_I2S_ReceiveData(int param_1)

{
  gp = &__global_pointer;
  return *(undefined2 *)(param_1 + 0xc);
}

