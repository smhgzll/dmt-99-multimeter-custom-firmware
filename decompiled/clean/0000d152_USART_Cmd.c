/* Semantic review: high. USART CTLR1 enable bit.
 * Address 0x0000d152; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d152; Ghidra pseudocode, not compilable reconstructed source. */

/* high: USART CTLR1 enable bit */

void USART_Cmd(int param_1,int param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *(ushort *)(param_1 + 0xc) & 0xdfff;
  }
  else {
    uVar1 = *(ushort *)(param_1 + 0xc) | 0x2000;
  }
  *(ushort *)(param_1 + 0xc) = uVar1;
  return;
}

