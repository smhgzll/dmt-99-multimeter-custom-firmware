/* Semantic review: high. ADC CTLR2 bit 0 enable.
 * Address 0x0000c74c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c74c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 bit 0 enable */

void ADC_Cmd(int param_1,int param_2)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xfffffffe;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 1;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}

