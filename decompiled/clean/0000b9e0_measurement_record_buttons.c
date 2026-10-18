/* Semantic review: high. Command construction plus history append from event 5.
 * Address 0x0000b9e0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000b9e0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Command construction plus history append from event 5 */

void measurement_record_buttons(void)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  byte bVar4;
  undefined4 extraout_a4;
  undefined8 uVar5;
  uchar auStack_1c [3];
  undefined1 uStack_19;
  undefined1 uStack_13;
  
  gp = &__global_pointer;
  _memcpy(auStack_1c,&DAT_ram_0001e82c,10);
  if (button_event == '\0') {
    return;
  }
  if (button_event != '\x03') {
    if (button_event == '\x04') {
      DAT_ram_20000200 = 0;
      DAT_ram_20000202 = '\0';
      bVar4 = DAT_ram_20000201 + 1;
      DAT_ram_20000201 = DAT_ram_20000201 + '\x01';
      if (4 < bVar4) {
        DAT_ram_20000201 = '\x01';
      }
      if (DAT_ram_20000201 == '\x01') {
        uStack_19 = 0x11;
        uStack_13 = 0x16;
      }
      else if (DAT_ram_20000201 == '\x02') {
        uStack_19 = 0x10;
        uStack_13 = 0x15;
      }
      else if (DAT_ram_20000201 == '\x03') {
        uStack_19 = 0x16;
        uStack_13 = 0x1b;
      }
      else {
        uStack_19 = 0x15;
        uStack_13 = 0x1a;
      }
    }
    else if (button_event == '\x02') {
      uStack_19 = 3;
      uStack_13 = 8;
    }
    else {
      if (button_event != '\x01') {
        if (button_event == '\x05') {
          button_event = '\0';
          if ((measurement_mode == '\x12') && (uVar1 = settings_temperature_access(0,0), uVar1 == 1)
             ) {
            uVar5 = FUN_ram_0001e2e2(measurement_value_bits);
            uVar5 = FUN_ram_0001c3b8((int)uVar5,(int)((ulonglong)uVar5 >> 0x20),0xcccccccd,
                                     0x3ffccccc);
            FUN_ram_0001b61e((int)uVar5,(int)((ulonglong)uVar5 >> 0x20),0,0x40400000);
            uVar2 = FUN_ram_0001cfae();
            FUN_ram_0000969e(&measurement_text,&DAT_ram_00023c2c,uVar2);
          }
          FUN_ram_00012c78(&measurement_text);
        }
        else if (button_event == '\x06') {
          button_event = 0;
          display_fill(0,2,extraout_a4);
          FUN_ram_0000fde6();
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
            iVar3 = settings_language_access(0,0);
            if (iVar3 == 0) {
              FUN_ram_00019366(1,1,0);
            }
          }
          do {
            iVar3 = menu_dispatch();
          } while (iVar3 != 1);
          DAT_ram_20000200 = 0;
          FUN_ram_0000b5ae();
          FUN_ram_000096c0();
          return;
        }
        goto switchD_ram_0000ba44_default;
      }
      DAT_ram_20000200 = 0;
      DAT_ram_20000201 = '\0';
      bVar4 = DAT_ram_20000202 + 1;
      DAT_ram_20000202 = DAT_ram_20000202 + '\x01';
      if (2 < bVar4) {
        DAT_ram_20000202 = button_event;
      }
      if (DAT_ram_20000202 == '\x01') {
        uStack_19 = 0x12;
        uStack_13 = 0x17;
      }
      else {
        uStack_19 = 0xf;
        uStack_13 = 0x14;
      }
    }
    goto LAB_ram_0000ba94;
  }
  DAT_ram_20000201 = '\0';
  DAT_ram_20000202 = '\0';
  DAT_ram_20000200 = DAT_ram_20000200 + 1;
  if (5 < DAT_ram_20000200) {
    DAT_ram_20000200 = 1;
  }
  button_event = '\0';
  switch(DAT_ram_20000200 - 1) {
  case 0:
    uStack_19 = 0xc;
    goto switchD_ram_0000ba44_default;
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
    goto switchD_ram_0000ba44_default;
  }
LAB_ram_0000ba94:
  button_event = '\0';
switchD_ram_0000ba44_default:
  measurement_send_command(auStack_1c);
  return;
}

