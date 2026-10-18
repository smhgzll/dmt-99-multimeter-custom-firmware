/* Semantic review: high. TIM channel 2 compare halfword write.
 * Address 0x0000d06e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d06e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM channel 2 compare halfword write */

void TIM_SetCompare2(int param_1,undefined2 param_2)

{
  gp = &__global_pointer;
  *(undefined2 *)(param_1 + 0x38) = param_2;
  return;
}

