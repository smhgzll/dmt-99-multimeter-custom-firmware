/* Original address: 0x0000c75c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 reset-calibration bit */

void ADC_ResetCalibration(int param_1)

{
  gp = &__global_pointer;
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 8;
  return;
}

