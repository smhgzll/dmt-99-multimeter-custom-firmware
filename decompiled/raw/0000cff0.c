/* Original address: 0x0000cff0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM CTLR1 enable bit */

void TIM_Cmd(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xfffe;
  }
  else {
    uVar1 = *param_1 | 1;
  }
  *param_1 = uVar1;
  return;
}

