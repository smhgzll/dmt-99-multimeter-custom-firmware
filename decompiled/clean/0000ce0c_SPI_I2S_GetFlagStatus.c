/* Semantic review: high. SPI status flag mask.
 * Address 0x0000ce0c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ce0c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI status flag mask */

bool SPI_I2S_GetFlagStatus(int param_1,ushort param_2)

{
  gp = &__global_pointer;
  return (*(ushort *)(param_1 + 8) & param_2) != 0;
}

