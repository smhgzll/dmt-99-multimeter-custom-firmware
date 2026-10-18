/* Original address: 0x0000c76e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 calibration bit */

void ADC_StartCalibration(int param_1)

{
  gp = &__global_pointer;
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 4;
  return;
}

