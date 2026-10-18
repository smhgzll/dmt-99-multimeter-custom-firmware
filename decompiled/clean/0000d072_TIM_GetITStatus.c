/* Semantic review: high. TIM interrupt enable and pending bits.
 * Address 0x0000d072; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d072; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM interrupt enable and pending bits */

bool TIM_GetITStatus(int param_1,ushort param_2)

{
  gp = &__global_pointer;
  if ((*(ushort *)(param_1 + 0x10) & param_2) != 0) {
    return (*(ushort *)(param_1 + 0xc) & param_2) != 0;
  }
  return false;
}

