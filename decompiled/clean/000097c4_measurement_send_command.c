/* Semantic review: high. Exactly ten USART1 bytes with transfer-complete polling.
 * Address 0x000097c4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000097c4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Exactly ten USART1 bytes with transfer-complete polling */

void measurement_send_command(uchar *command)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  iVar1 = 0;
  do {
    USART_SendData(&USART1,(ushort)command[iVar1]);
    do {
      uVar2 = USART_GetFlagStatus(&USART1,0x40);
    } while (uVar2 == 0);
    iVar1 = iVar1 + 1;
  } while (iVar1 != 10);
  return;
}

