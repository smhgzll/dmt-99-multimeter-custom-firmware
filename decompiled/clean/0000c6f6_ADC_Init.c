/* Semantic review: high. Register masks and struct offsets correspond to official WCH ADC_Init.
 * Address 0x0000c6f6; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c6f6; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Register masks and struct offsets correspond to official WCH ADC_Init */

void ADC_Init(int param_1,uint *param_2)

{
  gp = &__global_pointer;
  *(uint *)(param_1 + 4) =
       *param_2 | param_2[6] | param_2[7] | param_2[1] << 8 | *(uint *)(param_1 + 4) & 0xe0f0feff;
  *(uint *)(param_1 + 8) =
       param_2[4] | param_2[3] | param_2[2] << 1 | *(uint *)(param_1 + 8) & 0xfff1f7fd;
  *(uint *)(param_1 + 0x2c) =
       ((byte)param_2[5] - 1 & 0xff) << 0x14 | *(uint *)(param_1 + 0x2c) & 0xff0fffff;
  return;
}

