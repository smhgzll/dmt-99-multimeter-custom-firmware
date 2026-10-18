/* Original address: 0x0000c780; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 mask 0x500000 */

void ADC_SoftwareStartConvCmd(int param_1,int param_2)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xffafffff;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 0x500000;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}

