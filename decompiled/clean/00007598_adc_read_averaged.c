/* Semantic review: high. Repeated ADC conversion polling and averaging.
 * Address 0x00007598; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007598; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Repeated ADC conversion polling and averaging */

uint adc_read_averaged(undefined4 param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  ADC_RegularChannelConfig(&ADC1,param_1,1,7);
  uVar3 = 0;
  for (uVar1 = 0; uVar1 != param_2; uVar1 = uVar1 + 1 & 0xff) {
    ADC_SoftwareStartConvCmd(&ADC1,1);
    do {
      iVar2 = ADC_GetFlagStatus(&ADC1,2);
    } while (iVar2 == 0);
    iVar2 = ADC_GetConversionValue(&ADC1);
    uVar3 = uVar3 + iVar2;
    delay_ms(5);
  }
  return uVar3 / uVar1 & 0xffff;
}

