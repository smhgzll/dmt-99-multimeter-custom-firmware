/* Original address: 0x0000cd52; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC AHB clock enable register */

void RCC_AHBPeriphClockCmd(uint param_1,int param_2)

{
  uint32_t uVar1;
  
  gp = &__global_pointer;
  if (param_2 != 0) {
    uVar1 = RCC.AHBPCENR;
    RCC.AHBPCENR = param_1 | uVar1;
    return;
  }
  uVar1 = RCC.AHBPCENR;
  RCC.AHBPCENR = ~param_1 & uVar1;
  return;
}

