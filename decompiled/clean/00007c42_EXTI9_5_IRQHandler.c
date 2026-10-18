/* Semantic review: high. Vector slot 39; PC9 event 3 and PC8 short 5 long 6.
 * Address 0x00007c42; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007c42; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 39; PC9 event 3 and PC8 short 5 long 6 */

void EXTI9_5_IRQHandler(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  iVar2 = EXTI_GetITStatus(0x200);
  if (iVar2 != 0) {
    delay_ms(0x14);
    uVar3 = GPIO_ReadInputDataBit(&GPIOC,0x200);
    if (uVar3 == 0) {
      button_event = 3;
    }
  }
  iVar2 = EXTI_GetITStatus(0x100);
  if (iVar2 != 0) {
    delay_ms(0x14);
    uVar3 = GPIO_ReadInputDataBit(&GPIOC,0x100);
    if (uVar3 == 0) {
      button_event = 5;
      cVar1 = '=';
      do {
        uVar3 = GPIO_ReadInputDataBit(&GPIOC,0x100);
        if (uVar3 != 0) goto LAB_ram_00007cba;
        cVar1 = cVar1 + -1;
        delay_ms(10);
      } while (cVar1 != '\0');
      button_event = 6;
    }
  }
LAB_ram_00007cba:
  idle_seconds_counter = 0;
  EXTI_ClearITPendingBit(0x200);
  EXTI_ClearITPendingBit(0x100);
  return;
}

