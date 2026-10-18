/* Original address: 0x0000cd70; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC APB2 clock enable register */

void RCC_APB2PeriphClockCmd(uint param_1,int param_2)

{
  uint32_t uVar1;
  
  gp = &__global_pointer;
  if (param_2 != 0) {
    uVar1 = RCC.APB2PCENR;
    RCC.APB2PCENR = param_1 | uVar1;
    return;
  }
  uVar1 = RCC.APB2PCENR;
  RCC.APB2PCENR = ~param_1 & uVar1;
  return;
}

