/* Semantic review: high. USART2 transmit register and TC polling.
 * Address 0x0001a6e2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001a6e2; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: USART2 transmit register and TC polling */

int debug_putchar(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  for (iVar1 = 0; iVar1 < param_3; iVar1 = iVar1 + 1) {
    do {
      uVar2 = USART_GetFlagStatus(&USART2,0x40);
    } while (uVar2 == 0);
    USART_SendData(&USART2,(short)*(char *)(param_2 + iVar1));
  }
  return param_3;
}

