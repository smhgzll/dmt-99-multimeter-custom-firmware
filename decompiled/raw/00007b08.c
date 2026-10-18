/* Original address: 0x00007b08; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Polls five PC inputs at 10ms intervals with thresholds 41 and 101 */

undefined4 button_wait_longpress(int param_1)

{
  byte bVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  if (param_1 == 1) {
    bVar1 = 0;
    while (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x400), uVar2 == 0) {
      if (bVar1 == 0x65) {
        button_event = 0;
        return 1;
      }
      bVar1 = bVar1 + 1;
      delay_ms(10);
    }
  }
  else if (param_1 == 2) {
    bVar1 = 0;
    while (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x800), uVar2 == 0) {
      if (bVar1 == 0x65) {
        button_event = 0;
        return 1;
      }
      bVar1 = bVar1 + 1;
      delay_ms(10);
    }
  }
  else if (param_1 == 3) {
    bVar1 = 0;
    while (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x200), uVar2 == 0) {
      if (bVar1 == 0x65) {
        button_event = 0;
        return 1;
      }
      bVar1 = bVar1 + 1;
      delay_ms(10);
    }
  }
  else if (param_1 == 4) {
    bVar1 = 0;
    while (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x1000), uVar2 == 0) {
      if (bVar1 == 0x65) {
        button_event = 0;
        return 1;
      }
      bVar1 = bVar1 + 1;
      delay_ms(10);
    }
  }
  else {
    if (param_1 != 5) {
      return 0;
    }
    bVar1 = 0;
    while (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x100), uVar2 == 0) {
      if (bVar1 == 0x65) {
        button_event = 0;
        return 1;
      }
      bVar1 = bVar1 + 1;
      delay_ms(10);
    }
  }
  if (bVar1 < 0x29) {
    return 0;
  }
  button_event = 0;
  return 1;
}

