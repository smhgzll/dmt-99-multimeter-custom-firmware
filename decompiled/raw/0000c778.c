/* Original address: 0x0000c778; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 calibration status */

uint ADC_GetCalibrationStatus(int param_1)

{
  gp = &__global_pointer;
  return *(uint *)(param_1 + 8) >> 2 & 1;
}

