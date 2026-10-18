/* Original address: 0x0000cf42; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM channel 2 output compare registers */

void TIM_OC2Init(undefined *param_1,short *param_2)

{
  short sVar1;
  ushort uVar2;
  ushort uVar3;
  
  gp = &__global_pointer;
  *(ushort *)(param_1 + 0x20) = *(ushort *)(param_1 + 0x20) & 0xffef;
  uVar2 = *(ushort *)(param_1 + 4);
  sVar1 = *param_2;
  uVar3 = *(ushort *)(param_1 + 0x20) & 0xffdf | (param_2[1] | param_2[4]) << 4;
  if ((((param_1 == &TIM1) || (param_1 == &TIM8)) || (param_1 == &TIM9)) || (param_1 == &TIM10)) {
    uVar3 = (uVar3 & 0xff7f | param_2[5] << 4) & 0xffbf | param_2[2] << 4;
    uVar2 = uVar2 & 0xf3ff | (param_2[7] | param_2[6]) << 2;
  }
  *(ushort *)(param_1 + 4) = uVar2;
  *(ushort *)(param_1 + 0x18) = *(ushort *)(param_1 + 0x18) & 0x8cff | sVar1 << 8;
  *(short *)(param_1 + 0x38) = param_2[3];
  *(ushort *)(param_1 + 0x20) = uVar3;
  return;
}

