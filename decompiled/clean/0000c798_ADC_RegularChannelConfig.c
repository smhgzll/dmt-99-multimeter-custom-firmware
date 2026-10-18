/* Semantic review: high. ADC sample time and regular sequence register selection.
 * Address 0x0000c798; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c798; Ghidra pseudocode, not compilable reconstructed source. */

/* high: ADC sample time and regular sequence register selection */

void ADC_RegularChannelConfig(int param_1,uint param_2,uint param_3,int param_4)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if (param_2 < 10) {
    *(uint *)(param_1 + 0x10) =
         param_4 << (param_2 * 3 & 0x1f) | ~(7 << (param_2 * 3 & 0x1f)) & *(uint *)(param_1 + 0x10);
  }
  else {
    uVar1 = (param_2 - 10) * 3;
    *(uint *)(param_1 + 0xc) =
         param_4 << (uVar1 & 0x1f) | ~(7 << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0xc);
  }
  if (param_3 < 7) {
    uVar1 = (param_3 - 1) * 5;
    *(uint *)(param_1 + 0x34) =
         param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x34);
    return;
  }
  if (param_3 < 0xd) {
    uVar1 = (param_3 - 7) * 5;
    *(uint *)(param_1 + 0x30) =
         param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x30);
    return;
  }
  uVar1 = (param_3 - 0xd) * 5;
  *(uint *)(param_1 + 0x2c) =
       param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x2c);
  return;
}

