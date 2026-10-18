/* Semantic review: high. Button events select documented command IDs and menu state.
 * Address 0x0000b5c8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000b5c8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Button events select documented command IDs and menu state */

void measurement_regular_buttons(void)

{
  int iVar1;
  undefined4 uVar2;
  byte bVar3;
  uchar auStack_1c [3];
  undefined1 uStack_19;
  undefined1 uStack_13;
  
  gp = &__global_pointer;
  _memcpy(auStack_1c,&DAT_ram_0001e82c,10);
  if (button_event == 0) goto LAB_ram_0000b764;
  if (button_event != 3) {
    if (button_event == 4) {
      bVar3 = DAT_ram_200001fd + 1;
      DAT_ram_200001fd = DAT_ram_200001fd + '\x01';
      if (4 < bVar3) {
        DAT_ram_200001fd = '\x01';
      }
      DAT_ram_200001fc = 0;
      DAT_ram_200001fe = 0;
      if (DAT_ram_200001fd == '\x01') {
        uStack_19 = 0x11;
        uStack_13 = 0x16;
      }
      else if (DAT_ram_200001fd == '\x02') {
        uStack_19 = 0x10;
        uStack_13 = 0x15;
      }
      else if (DAT_ram_200001fd == '\x03') {
        uStack_19 = 0x16;
        uStack_13 = 0x1b;
      }
      else {
        uStack_19 = 0x15;
        uStack_13 = 0x1a;
      }
      goto LAB_ram_0000b80c;
    }
    if (button_event == 2) {
      button_event = 0;
      iVar1 = button_wait_longpress(2);
      if (iVar1 == 1) {
        if (((((measurement_mode == '\f') || (measurement_mode == '\r')) ||
             (measurement_mode == '\x10')) ||
            ((measurement_mode == '\x11' || (measurement_mode == '\x15')))) ||
           ((measurement_mode == '\x16' || (measurement_mode == '\n')))) {
          if (DAT_ram_200001f4 == '\0') {
            uVar2 = 0xca;
            if (display_layout != '\0') {
              uVar2 = 0x40;
            }
            resource_draw_jpeg("mode/rel.jpg",0xcd,uVar2);
            FUN_ram_00007918();
            DAT_ram_200001f4 = '\x01';
            DAT_ram_200001f5 = measurement_mode;
            DAT_ram_200001f6 = measurement_unit_code;
            DAT_ram_200001f8 = measurement_value_bits;
          }
          else {
            if (display_layout == '\0') {
              FUN_ram_0001a360(&DAT_ram_00026398,0xcd,0xca,0,0x4228);
            }
            else {
              display_fill_rectangle(0xcd,0x40,0xf1,0x4f,0);
            }
            FUN_ram_00007918();
            DAT_ram_200001f4 = '\0';
          }
        }
      }
      else if ((measurement_mode != '\x13') && (measurement_mode != '\t')) {
        uStack_19 = 3;
        uStack_13 = 8;
      }
    }
    else {
      if (button_event == 1) {
        bVar3 = DAT_ram_200001fe + 1;
        DAT_ram_200001fe = DAT_ram_200001fe + 1;
        if (4 < bVar3) {
          DAT_ram_200001fe = button_event;
        }
        DAT_ram_200001da = 0x19;
        DAT_ram_200001fc = 0;
        DAT_ram_200001fd = '\0';
        DAT_ram_200001ff = '\0';
        button_event = 0;
        if (DAT_ram_200001fe == 2) {
          uStack_19 = 0xf;
          uStack_13 = 0x14;
        }
        else if (DAT_ram_200001fe < 3) {
          if (DAT_ram_200001fe != 1) goto switchD_ram_0000b636_default;
          uStack_19 = 0x12;
          uStack_13 = 0x17;
        }
        else if (DAT_ram_200001fe == 3) {
          uStack_19 = 9;
          uStack_13 = 0xe;
        }
        else {
          if (DAT_ram_200001fe != 4) goto switchD_ram_0000b636_default;
          uStack_19 = 0x13;
          uStack_13 = 0x18;
        }
        goto LAB_ram_0000b80c;
      }
      if (button_event != 5) {
        if (button_event == 6) {
          DAT_ram_200001da = 0x19;
          measurement_mode = 0;
          button_event = 0;
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
          FUN_ram_0000b5ae();
          return;
        }
        if (button_event == 7) {
          DAT_ram_200001da = 0x19;
          button_event = 0;
        }
        goto switchD_ram_0000b636_default;
      }
      bVar3 = DAT_ram_200001ff + 1;
      DAT_ram_200001ff = DAT_ram_200001ff + '\x01';
      if (0xc < bVar3) {
        DAT_ram_200001ff = '\x01';
      }
      uStack_13 = 0x19;
      DAT_ram_200001da = 0x19;
      DAT_ram_200001fc = 0;
      DAT_ram_200001fd = '\0';
      DAT_ram_200001fe = 0;
      uStack_19 = 0x14;
    }
    button_event = 0;
    goto switchD_ram_0000b636_default;
  }
  DAT_ram_200001fc = DAT_ram_200001fc + 1;
  if (5 < DAT_ram_200001fc) {
    DAT_ram_200001fc = 1;
  }
  DAT_ram_200001da = 0x19;
  DAT_ram_200001fd = '\0';
  DAT_ram_200001fe = 0;
  DAT_ram_200001ff = '\0';
  button_event = 0;
  switch(DAT_ram_200001fc - 1) {
  case 0:
    uStack_19 = 0xc;
    goto switchD_ram_0000b636_default;
  case 1:
    uStack_19 = 0xd;
    uStack_13 = 0x12;
    break;
  case 2:
    uStack_19 = 0xb;
    uStack_13 = 0x10;
    break;
  case 3:
    uStack_19 = 10;
    uStack_13 = 0xf;
    break;
  case 4:
    uStack_19 = 0xe;
    uStack_13 = 0x13;
    break;
  default:
    goto switchD_ram_0000b636_default;
  }
LAB_ram_0000b80c:
  DAT_ram_200001ff = '\0';
  button_event = 0;
  DAT_ram_200001da = 0x19;
switchD_ram_0000b636_default:
  measurement_send_command(auStack_1c);
LAB_ram_0000b764:
  if ((DAT_ram_200001f4 == '\x01') &&
     ((DAT_ram_200001f5 != measurement_mode || (measurement_unit_code != DAT_ram_200001f6)))) {
    DAT_ram_200001f4 = '\0';
    if (display_layout == '\0') {
      FUN_ram_0001a360(&DAT_ram_00026398,0xcd,0xca,0,0x4228);
    }
    else {
      display_fill_rectangle(0xcd,0x40,0xf1,0x4f,0);
    }
  }
  return;
}

