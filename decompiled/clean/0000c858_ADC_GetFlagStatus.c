/* Semantic review: high. ADC status mask test.
 * Address 0x0000c858; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c858; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC status mask test */

bool ADC_GetFlagStatus(uint *param_1,uint param_2)

{
  gp = &__global_pointer;
  return (*param_1 & param_2) != 0;
}

