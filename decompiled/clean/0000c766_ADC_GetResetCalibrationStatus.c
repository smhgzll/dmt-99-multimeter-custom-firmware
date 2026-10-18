/* Semantic review: high. ADC CTLR2 reset-calibration status.
 * Address 0x0000c766; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c766; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC CTLR2 reset-calibration status */

uint ADC_GetResetCalibrationStatus(int param_1)

{
  gp = &__global_pointer;
  return *(uint *)(param_1 + 8) >> 3 & 1;
}

