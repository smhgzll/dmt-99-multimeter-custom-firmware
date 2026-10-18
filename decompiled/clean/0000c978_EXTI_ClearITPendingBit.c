/* Semantic review: high. EXTI pending write-one-to-clear.
 * Address 0x0000c978; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c978; Ghidra pseudocode, not compilable reconstructed source. */

/* high: EXTI pending write-one-to-clear */

void EXTI_ClearITPendingBit(undefined4 param_1)

{
  gp = &__global_pointer;
  DAT_ram_40010414 = param_1;
  return;
}

