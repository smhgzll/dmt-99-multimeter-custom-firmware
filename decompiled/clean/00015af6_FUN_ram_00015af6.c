/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00015af6; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00015af6; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

undefined4 FUN_ram_00015af6(void)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  short value;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  gp = &__global_pointer;
  if (button_event != '\x05') {
    return 0;
  }
  switch(DAT_ram_200001d8 + -2) {
  case '\0':
    application_view = 1;
    settings_theme_access(1,1);
    DAT_ram_200001d8 = 1;
    return 0;
  case '\x01':
    application_view = 2;
    value = 2;
    goto LAB_ram_00015b46;
  case '\x02':
    application_view = 3;
    value = 3;
LAB_ram_00015b46:
    settings_theme_access(1,value);
    DAT_ram_200001d8 = 1;
    return 0;
  default:
    goto LAB_ram_0000664a;
  case '\x04':
    uVar6 = 6;
    break;
  case '\x05':
    uVar6 = 7;
    break;
  case '\x06':
    uVar6 = 8;
    break;
  case '\a':
    uVar6 = 9;
    break;
  case '\b':
    uVar6 = 10;
    break;
  case '\t':
    uVar6 = 0xb;
    break;
  case '\f':
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) goto LAB_ram_00015d14;
    uVar6 = 3;
    if (display_layout != '\0') {
      display_fill_rectangle(0,0,0x82,0xef,0);
      display_fill_rectangle(0x8b,9,0xd1,0x21,0);
      display_fill_rectangle(0x8b,0x7d,0xf9,0x95,0);
      display_fill_rectangle(0xb4,0x26,0x118,0x3e,0);
      display_fill_rectangle(0xb4,0x52,0x136,0x6a,0);
      FUN_ram_000135b0(1,3);
      FUN_ram_00019366(1,4,0);
      goto LAB_ram_00015d14;
    }
    goto LAB_ram_00015d30;
  case '\r':
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 1) goto LAB_ram_00015d14;
    if (display_layout != '\0') {
      display_fill_rectangle(0x8b,9,0xd1,0x21,0);
      display_fill_rectangle(0x8b,0x7d,0xf9,0x95,0);
      display_fill_rectangle(0xb4,0x26,0x118,0x3e,0);
      display_fill_rectangle(0xb4,0x52,0x136,0x6a,0);
      FUN_ram_000135b0(1,4);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode_set.jpg")
      ;
      resource_draw_jpeg(&DAT_ram_20003304,0,0);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"install.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0,0x78);
      goto LAB_ram_00015d14;
    }
    uVar6 = 4;
LAB_ram_00015d30:
    FUN_ram_00013c02(1,uVar6);
    display_fill_rectangle(0,0x1d,0x78,0xd2,0);
    display_fill_rectangle(0xb3,0x10,0x114,0x24,0);
    display_fill_rectangle(0xb3,0x66,0x114,0x79,0);
    display_fill_rectangle(0x9d,0x47,0x11d,0x59,0);
    FUN_ram_00018ad2(1,4,0);
LAB_ram_00015d14:
    FUN_ram_000153d0();
    button_event = 0;
    return 0;
  case '\x12':
    uVar2 = settings_layout_access(0,0);
    if (uVar2 != 1) {
      button_event = 0;
      return 0;
    }
    settings_layout_access(1,0);
    display_fill(0);
    display_layout = 0;
    resource_draw_jpeg("motif2/menu_adorn1.jpg",0,0);
    resource_draw_jpeg("motif2/menu_adorn2.jpg",0,0xd3);
    FUN_ram_00018ad2(1,4,0);
    FUN_ram_000153d0();
    uVar7 = 0x1c9f;
    uVar6 = 0x5b;
    uVar4 = 0xd0;
    uVar5 = 0x2f;
    uVar1 = 0x9b;
    goto LAB_ram_00015bdc;
  case '\x13':
    uVar2 = settings_layout_access(0,0);
    if (uVar2 != 0) {
      button_event = 0;
      return 0;
    }
    settings_layout_access(1,1);
    display_fill(0);
    display_layout = 1;
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00019366(1,4,0);
    }
    else {
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode_set.jpg")
      ;
      resource_draw_jpeg(&DAT_ram_20003304,0,0);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"install.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0,0x78);
    }
    FUN_ram_000153d0();
    uVar7 = 0xfd40;
    uVar6 = 0x6f;
    uVar4 = 0x135;
    uVar5 = 0x4e;
    uVar1 = 0x8d;
LAB_ram_00015bdc:
    FUN_ram_00008194(uVar1,uVar5,uVar4,uVar6,uVar7);
    button_event = 0;
    return 0;
  case '\x14':
    goto switchD_ram_00015b26_caseD_14;
  case '\x15':
    uVar6 = 2;
    goto LAB_ram_00015e4c;
  case '\x16':
    uVar6 = 3;
LAB_ram_00015e4c:
    uVar4 = 2;
    goto LAB_ram_00015e4e;
  case '\x17':
    uVar6 = 1;
    goto LAB_ram_00015e64;
  case '\x18':
    uVar6 = 2;
    goto LAB_ram_00015e64;
  case '\x19':
    uVar6 = 3;
    goto LAB_ram_00015e64;
  case '\x1a':
    uVar6 = 4;
LAB_ram_00015e64:
    uVar4 = 3;
LAB_ram_00015e4e:
    if (display_layout == '\0') {
      FUN_ram_00013c02();
    }
    else {
      FUN_ram_000135b0(uVar4,uVar6);
    }
switchD_ram_00015b26_caseD_14:
    button_event = 0;
    return 0;
  case '\x1c':
    DAT_ram_200001d8 = 0x1d;
    button_event = 0;
    return 0;
  case '\x1d':
    DAT_ram_200001d8 = 1;
    button_event = 0;
    settings_restore_defaults();
                    /* WARNING: Subroutine does not return */
    application_loop();
  }
  FUN_ram_00011086(uVar6);
LAB_ram_0000664a:
  return 0;
}

