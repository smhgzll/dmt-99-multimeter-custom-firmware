/* Original address: 0x0000ce16; Ghidra pseudocode, not compilable reconstructed source. */

/* high: TIM prescaler auto-reload and update generation */

void TIM_TimeBaseInit(ushort *param_1,ushort *param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  uVar1 = *param_1;
  if ((((param_1 == (ushort *)&TIM1) || (param_1 == (ushort *)&TIM2)) ||
      (param_1 == (ushort *)&TIM3)) ||
     (((param_1 == (ushort *)&TIM4 || (param_1 == (ushort *)&TIM5)) ||
      ((param_1 == (ushort *)&TIM8 ||
       ((param_1 == (ushort *)&TIM9 || (param_1 == (ushort *)&TIM10)))))))) {
    uVar1 = uVar1 & 0xff8f | param_2[1];
  }
  if ((param_1 != (ushort *)&TIM6) && (param_1 != (ushort *)&TIM7)) {
    uVar1 = uVar1 & 0xfcff | param_2[3];
  }
  *param_1 = uVar1;
  param_1[0x16] = param_2[2];
  param_1[0x14] = *param_2;
  if ((((param_1 == (ushort *)&TIM1) || (param_1 == (ushort *)&TIM8)) ||
      (param_1 == (ushort *)&TIM9)) || (param_1 == (ushort *)&TIM10)) {
    param_1[0x18] = (ushort)(byte)param_2[4];
  }
  param_1[10] = 1;
  return;
}

