/* Original address: 0x00007c0a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 24; PC2 debounce event 8 and conditional power-off */

void EXTI2_IRQHandler(void)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  iVar1 = EXTI_GetITStatus(4);
  if (iVar1 != 0) {
    delay_ms(0x14);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,4);
    if ((uVar2 == 0) && (normal_operation == '\x01')) {
                    /* WARNING: Subroutine does not return */
      power_down();
    }
    button_event = 8;
  }
  EXTI_ClearITPendingBit(4);
  return;
}

