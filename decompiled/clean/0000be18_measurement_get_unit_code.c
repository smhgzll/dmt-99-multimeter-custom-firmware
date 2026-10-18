/* Semantic review: high. Returns 0x20000289.
 * Address 0x0000be18; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000be18; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Returns 0x20000289 */

undefined1 measurement_get_unit_code(void)

{
  gp = &__global_pointer;
  return measurement_unit_code;
}

