/* Original address: 0x0000be1e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Returns raw 32-bit float representation at 0x20000284 */

undefined4 measurement_get_value_bits(void)

{
  gp = &__global_pointer;
  return measurement_value_bits;
}

