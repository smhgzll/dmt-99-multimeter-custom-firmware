/* Semantic review: high. Official USART struct bit masks and baud divisor.
 * Address 0x0000d096; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d096; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Official USART struct bit masks and baud divisor */

void USART_Init(USART_Registers *usart,USART_Init_Recovered *configuration)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  undefined1 auStack_24 [8];
  int iStack_1c;
  int iStack_18;
  
  gp = &__global_pointer;
  uVar1 = configuration->flow_control;
  usart->CTLR2 = usart->CTLR2 & 0xcfff | configuration->stop_bits;
  usart->CTLR1 = usart->CTLR1 & 0xe9f3 | configuration->word_length | configuration->parity |
                 configuration->mode;
  usart->CTLR3 = usart->CTLR3 & 0xfcff | uVar1;
  RCC_GetClocksFreq(auStack_24);
  if (usart == &USART1) {
    iStack_1c = iStack_18;
  }
  if ((short)usart->CTLR1 < 0) {
    uVar2 = configuration->baud << 1;
  }
  else {
    uVar2 = configuration->baud << 2;
  }
  uVar2 = (uint)(iStack_1c * 0x19) / uVar2;
  uVar3 = uVar2 % 100;
  if ((short)usart->CTLR1 < 0) {
    uVar1 = (ushort)((uVar3 * 8 + 0x32) / 100) & 7;
  }
  else {
    uVar1 = (ushort)((uVar3 * 0x10 + 0x32) / 100) & 0xf;
  }
  usart->BRR = uVar1 | (ushort)(uVar2 / 100 << 4);
  return;
}

