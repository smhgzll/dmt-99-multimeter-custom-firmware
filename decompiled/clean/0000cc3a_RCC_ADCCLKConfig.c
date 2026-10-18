/* Semantic review: high. RCC CFGR0 ADC prescaler field.
 * Address 0x0000cc3a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cc3a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC CFGR0 ADC prescaler field */

void RCC_ADCCLKConfig(uint param_1)

{
  uint32_t uVar1;
  
  gp = &__global_pointer;
  uVar1 = RCC.CFGR0;
  RCC.CFGR0 = param_1 | uVar1 & 0xffff3fff;
  return;
}

