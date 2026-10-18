/* Semantic review: high. TIM CHCTLR1 second channel preload.
 * Address 0x0000d058; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d058; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM CHCTLR1 second channel preload */

void TIM_OC2PreloadConfig(int param_1,short param_2)

{
  gp = &__global_pointer;
  *(ushort *)(param_1 + 0x18) = *(ushort *)(param_1 + 0x18) & 0xf7ff | param_2 << 8;
  return;
}

