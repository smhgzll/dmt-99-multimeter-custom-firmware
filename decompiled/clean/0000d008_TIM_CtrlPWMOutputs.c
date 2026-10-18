/* Semantic review: high. TIM BDTR master output enable.
 * Address 0x0000d008; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d008; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM BDTR master output enable */

void TIM_CtrlPWMOutputs(int param_1,int param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = (ushort)(((uint)*(ushort *)(param_1 + 0x44) << 0x11) >> 0x11);
  }
  else {
    uVar1 = *(ushort *)(param_1 + 0x44) | 0x8000;
  }
  *(ushort *)(param_1 + 0x44) = uVar1;
  return;
}

