/* Original address: 0x0000cd8e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: RCC APB1 clock enable register */

void RCC_APB1PeriphClockCmd(uint param_1,int param_2)

{
  uint32_t uVar1;
  
  gp = &__global_pointer;
  if (param_2 != 0) {
    uVar1 = RCC.APB1PCENR;
    RCC.APB1PCENR = param_1 | uVar1;
    return;
  }
  uVar1 = RCC.APB1PCENR;
  RCC.APB1PCENR = ~param_1 & uVar1;
  return;
}

