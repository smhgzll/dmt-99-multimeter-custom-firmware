/* Semantic review: high. Vector slot 2; mret.
 * Address 0x0000cb88; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb88; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 2; mret */

void NMI_Handler(void)

{
  gp = &__global_pointer;
  return;
}

