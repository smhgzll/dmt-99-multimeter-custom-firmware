/* Original address: 0x0000cec0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM channel 1 output compare registers */

void TIM_OC1Init(undefined *param_1,ushort *param_2)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  
  gp = &__global_pointer;
  uVar1 = *param_2;
  uVar3 = param_2[1];
  *(ushort *)(param_1 + 0x20) = *(ushort *)(param_1 + 0x20) & 0xfffe;
  uVar2 = *(ushort *)(param_1 + 4);
  uVar3 = *(ushort *)(param_1 + 0x20) & 0xfffd | param_2[4] | uVar3;
  if ((((param_1 == &TIM1) || (param_1 == &TIM8)) || (param_1 == &TIM9)) || (param_1 == &TIM10)) {
    uVar3 = (uVar3 & 0xfff7 | param_2[5]) & 0xfffb | param_2[2];
    uVar2 = uVar2 & 0xfcff | param_2[6] | param_2[7];
  }
  *(ushort *)(param_1 + 4) = uVar2;
  *(ushort *)(param_1 + 0x18) = *(ushort *)(param_1 + 0x18) & 0xff8c | uVar1;
  *(ushort *)(param_1 + 0x34) = param_2[3];
  *(ushort *)(param_1 + 0x20) = uVar3;
  return;
}

