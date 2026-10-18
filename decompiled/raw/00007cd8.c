/* Original address: 0x00007cd8; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 56; PC10 11 12 events 1 2 4 */

void EXTI15_10_IRQHandler(void)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  iVar1 = EXTI_GetITStatus(0x400);
  if (iVar1 != 0) {
    delay_ms(0x14);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x400);
    if (uVar2 == 0) {
      button_event = 1;
    }
  }
  iVar1 = EXTI_GetITStatus(0x800);
  if (iVar1 != 0) {
    delay_ms(0x14);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x800);
    if (uVar2 == 0) {
      button_event = 2;
    }
  }
  iVar1 = EXTI_GetITStatus(0x1000);
  if (iVar1 != 0) {
    delay_ms(0x14);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x1000);
    if (uVar2 == 0) {
      button_event = 4;
    }
  }
  idle_seconds_counter = 0;
  EXTI_ClearITPendingBit(0x400);
  EXTI_ClearITPendingBit(0x800);
  EXTI_ClearITPendingBit(0x1000);
  return;
}

