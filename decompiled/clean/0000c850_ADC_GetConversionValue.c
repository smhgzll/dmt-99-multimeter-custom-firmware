/* Semantic review: high. ADC data register halfword read.
 * Address 0x0000c850; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c850; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC data register halfword read */

uint ADC_GetConversionValue(int param_1)

{
  gp = &__global_pointer;
  return *(uint *)(param_1 + 0x4c) & 0xffff;
}

