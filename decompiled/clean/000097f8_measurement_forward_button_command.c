/* Semantic review: high. Builds AA55 template for IDs 1 3 8 9 from button events.
 * Address 0x000097f8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000097f8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Builds AA55 template for IDs 1 3 8 9 from button events */

void measurement_forward_button_command(void)

{
  char cVar1;
  uint uVar2;
  uchar auStack_1c [3];
  char cStack_19;
  undefined1 uStack_13;
  
  gp = &__global_pointer;
  _memcpy(auStack_1c,&DAT_ram_0001e820,10);
  cVar1 = button_event;
  if (button_event == '\x04') {
    delay_ms(10);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x1000);
    if (uVar2 != 0) {
      button_event = 0;
      return;
    }
    cStack_19 = '\x01';
    uStack_13 = 6;
  }
  else if (button_event == '\x03') {
    delay_ms(10);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x200);
    if (uVar2 != 0) {
      button_event = 0;
      return;
    }
    uStack_13 = 8;
    cStack_19 = cVar1;
  }
  else if (button_event == '\x02') {
    delay_ms(10);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x800);
    if (uVar2 != 0) {
      button_event = 0;
      return;
    }
    cStack_19 = '\b';
    uStack_13 = 0xd;
  }
  else {
    if (button_event != '\x01') {
      gp = &__global_pointer;
      return;
    }
    delay_ms(10);
    uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x400);
    if (uVar2 != 0) {
      button_event = 0;
      return;
    }
    cStack_19 = '\t';
    uStack_13 = 0xe;
  }
  measurement_send_command(auStack_1c);
  button_event = 0;
  return;
}

