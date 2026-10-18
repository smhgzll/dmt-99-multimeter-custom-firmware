/* Semantic review: high. Encoded USART interrupt enable and status tests.
 * Address 0x0000d1d6; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d1d6; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Encoded USART interrupt enable and status tests */

bool USART_GetITStatus(ushort *param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = (param_2 & 0xff) >> 5;
  if (uVar2 == 1) {
    uVar2 = (uint)param_1[6];
  }
  else if (uVar2 == 2) {
    uVar2 = (uint)param_1[8];
  }
  else {
    uVar2 = (uint)param_1[10];
  }
  bVar1 = false;
  if ((1 << (param_2 & 0x1f) & uVar2) != 0) {
    bVar1 = ((uint)*param_1 & 1 << (param_2 >> 8 & 0x1f)) != 0;
  }
  return bVar1;
}

