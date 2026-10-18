/* Original address: 0x0000bc90; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: Third application branch command construction */

void measurement_monitor_buttons(void)

{
  int iVar1;
  uchar auStack_1c [3];
  char cStack_19;
  char cStack_13;
  
  gp = &__global_pointer;
  _memcpy(auStack_1c,&DAT_ram_0001e82c,10);
  if (button_event == '\0') {
    return;
  }
  if (button_event == '\x03') {
    if (DAT_ram_20000203 != '\0') {
      cStack_19 = '\r';
    }
    else {
      cStack_19 = '\f';
    }
    DAT_ram_20000203 = DAT_ram_20000203 == '\0';
  }
  else {
    if (button_event != '\x04') {
      if (button_event == '\x02') {
        cStack_19 = '\x03';
        cStack_13 = '\b';
      }
      else {
        if (button_event != '\x01') {
          if (button_event == '\x06') {
            button_event = 0;
            buzzer_enable(0);
            display_fill(0);
            if (display_layout == '\0') {
              resource_draw_jpeg("motif2/menu_adorn1.jpg",0,0);
              resource_draw_jpeg("motif2/menu_adorn2.jpg",0,0xd3);
              FUN_ram_00008050(0,0x1c,0x78,0x1d,0x4228);
              FUN_ram_00008050(0x78,0x1c,0xa0,7,0x4228);
              FUN_ram_00008050(0xa0,7,0x140,7,0x4228);
              FUN_ram_00008050(0,0xd3,0x78,0xd2,0x4228);
              FUN_ram_00008050(0x78,0xd3,0xa0,0xe8,0x4228);
              FUN_ram_00008050(0xa0,0xe8,0x140,0xe8,0x4228);
              FUN_ram_00018ad2(1,1,0);
            }
            else {
              iVar1 = settings_language_access(0,0);
              if (iVar1 == 0) {
                FUN_ram_00019366(1,1,0);
              }
            }
            do {
              iVar1 = menu_dispatch();
            } while (iVar1 != 1);
            DAT_ram_20000203 = 0;
            DAT_ram_20000204 = 0;
            FUN_ram_0000b5ae();
            return;
          }
          goto LAB_ram_0000bcd0;
        }
        cStack_19 = '\x12';
        cStack_13 = '\x17';
      }
      button_event = '\0';
      goto LAB_ram_0000bcd0;
    }
    cStack_19 = '\x10';
    if (measurement_mode == '\x10') {
      cStack_19 = '\x15';
    }
  }
  cStack_13 = cStack_19 + '\x05';
  button_event = '\0';
LAB_ram_0000bcd0:
  measurement_send_command(auStack_1c);
  return;
}

