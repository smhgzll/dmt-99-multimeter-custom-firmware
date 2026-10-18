/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001598c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001598c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0001598c(void)

{
  undefined1 uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  uVar1 = DAT_ram_200001dd;
  gp = &__global_pointer;
  if ((button_event == '\x03') || (button_event == '\x04')) {
    if (button_event == '\x03') {
      if (DAT_ram_200001d8 == 0xc) {
        FUN_ram_00014a92();
      }
      else if (DAT_ram_200001d8 < 0xd) {
        if (DAT_ram_200001d8 == 1) {
          DAT_ram_200001d8 = 2;
        }
        else if (DAT_ram_200001d8 == 5) {
          DAT_ram_200001d8 = 6;
        }
      }
      else if (DAT_ram_200001d8 == 0x16) {
        if (display_layout == '\0') {
          uVar2 = settings_field_3_4_access(0,0);
          FUN_ram_0001432e(uVar2 + 1);
        }
        else {
          uVar2 = settings_field_3_4_access(0,0);
          FUN_ram_000142a4(1,uVar1,uVar2 + 1);
        }
        FUN_ram_00008016();
      }
      else if (DAT_ram_200001d8 == 0x1d) {
        DAT_ram_200001d8 = 0x1e;
      }
      else if (DAT_ram_200001d8 == 0xd) {
        FUN_ram_00014408(1);
        DAT_ram_200001d8 = 0x14;
      }
      if (button_event != '\x06') {
        button_event = '\0';
      }
    }
    else if (button_event == '\x04') {
      switch(DAT_ram_200001d8 - 2) {
      case 0:
      case 1:
      case 2:
        DAT_ram_200001d8 = 1;
        break;
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        DAT_ram_200001d8 = 5;
        break;
      case 0xc:
      case 0xd:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
      case 0x18:
      case 0x19:
      case 0x1a:
        if (display_layout == '\0') {
          display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
          display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
          uVar6 = 0xe4;
          uVar5 = 0x13f;
          uVar4 = 0xb;
          uVar3 = 0x13d;
        }
        else {
          display_fill_rectangle(0x83,0,0x140,0xf0,0);
          uVar6 = 0xea;
          uVar5 = 0x13d;
          uVar4 = 5;
          uVar3 = 0x13b;
        }
        display_fill_rectangle(uVar3,uVar4,uVar5,uVar6,0);
        FUN_ram_000153d0();
        DAT_ram_200001dd = 1;
        DAT_ram_200001d8 = 0xd;
        break;
      case 0x1c:
      case 0x1d:
        DAT_ram_200001d8 = 0x1d;
      }
    }
  }
  return 0;
}

