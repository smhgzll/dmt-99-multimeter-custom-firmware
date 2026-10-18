/* Semantic review: medium. Thirty-two state switch with settings history and about branches.
 * Address 0x000169d4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000169d4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* medium: Thirty-two state switch with settings history and about branches */

char menu_dispatch(void)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  byte bVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  char *pcVar8;
  undefined4 uVar9;
  
  gp = &__global_pointer;
  if (button_event == '\x06') {
    DAT_ram_200001db = 0xff;
    display_fill(0);
    FUN_ram_00014ef2();
    DAT_ram_200001d8 = 1;
    button_event = 0;
    DAT_ram_200001dd = 1;
    FUN_ram_0000fdf6();
    button_event = 0;
    return '\x01';
  }
  FUN_ram_000164d4();
  FUN_ram_0001598c();
  FUN_ram_00015af6();
  cVar1 = display_layout;
  if (DAT_ram_200001da == DAT_ram_200001d8) {
    return '\0';
  }
  DAT_ram_200001da = DAT_ram_200001d8;
  switch(DAT_ram_200001d8) {
  case '\x01':
    if (display_layout == '\0') {
      display_fill_rectangle(0x82,0x28,0x96,0xd0);
      uVar3 = 0xda;
      uVar9 = 0x14;
      uVar7 = 0x8c;
    }
    else {
      iVar4 = settings_language_access(0,0);
      if (iVar4 != 0) {
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,
                         "mode_set.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0,0);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0,0);
      }
      uVar3 = 0xf0;
      uVar9 = 0;
      uVar7 = 0x83;
    }
    display_fill_rectangle(uVar7,uVar9,0x140,uVar3,0);
    cVar1 = display_layout;
    if (application_view == '\x01') {
      if (display_layout == '\0') {
        uVar3 = 1;
        goto LAB_ram_00016a92;
      }
      iVar4 = settings_language_access(0,0);
      if (iVar4 == 0) {
        uVar3 = 1;
        goto LAB_ram_00016b98;
      }
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode1_.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0x9d,0x35);
      pcVar8 = "mode2.jpg";
LAB_ram_00016b4a:
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,pcVar8);
      resource_draw_jpeg(&DAT_ram_20003304,0x9d,0x67);
      pcVar8 = "mode3.jpg";
    }
    else {
      if (application_view == '\x02') {
        if (display_layout == '\0') {
          uVar3 = 2;
          goto LAB_ram_00016a92;
        }
        iVar4 = settings_language_access(0,0);
        if (iVar4 == 0) {
          uVar3 = 2;
          goto LAB_ram_00016b98;
        }
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode1.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0x9d,0x35);
        pcVar8 = "mode2_.jpg";
        goto LAB_ram_00016b4a;
      }
      if (display_layout == '\0') {
        uVar3 = 3;
LAB_ram_00016a92:
        FUN_ram_00019ba8(1,uVar3,0);
        return cVar1;
      }
      iVar4 = settings_language_access(0,0);
      if (iVar4 == 0) {
        uVar3 = 3;
LAB_ram_00016b98:
        FUN_ram_00019954(1,uVar3,0);
        return '\0';
      }
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode1.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0x9d,0x35);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode2.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0x9d,0x67);
      pcVar8 = "mode3_.jpg";
    }
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,pcVar8);
    uVar3 = 0x99;
    uVar7 = 0x9d;
    goto LAB_ram_00016b8a;
  case '\x02':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x34,0x108,0x57,0x1c9f);
      uVar3 = 0x89;
      uVar7 = 0x66;
LAB_ram_00016ca6:
      FUN_ram_00008194(0x9c,uVar7,0x108,uVar3,0);
      uVar3 = 0;
      uVar7 = 0xbb;
      uVar9 = 0x108;
      uVar6 = 0x98;
LAB_ram_00016cbc:
      uVar2 = 0x9c;
LAB_ram_00016cc0:
      FUN_ram_00008194(uVar2,uVar6,uVar9,uVar7,uVar3);
      return cVar1;
    }
    FUN_ram_00008194(0x9c,0x34,0x126,0x57,0xfd40);
    uVar3 = 0x89;
    uVar7 = 0x66;
    goto LAB_ram_00016cee;
  case '\x03':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x66,0x108,0x89,0x1c9f);
      uVar3 = 0x57;
      uVar7 = 0x34;
      goto LAB_ram_00016ca6;
    }
    FUN_ram_00008194(0x9c,0x66,0x126,0x89,0xfd40);
    uVar3 = 0x57;
    uVar7 = 0x34;
LAB_ram_00016cee:
    FUN_ram_00008194(0x9c,uVar7,0x126,uVar3,0);
    uVar3 = 0xbb;
    uVar7 = 0x98;
LAB_ram_00016d04:
    uVar6 = 0x126;
    uVar9 = 0x9c;
LAB_ram_00016d08:
    FUN_ram_00008194(uVar9,uVar7,uVar6,uVar3,0);
    return '\0';
  case '\x04':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x98,0x108,0xbb,0x1c9f);
      FUN_ram_00008194(0x9c,0x34,0x108,0x57,0);
      uVar3 = 0;
      uVar7 = 0x89;
      uVar9 = 0x108;
      uVar6 = 0x66;
      goto LAB_ram_00016cbc;
    }
    FUN_ram_00008194(0x9c,0x98,0x126,0xbb,0xfd40);
    FUN_ram_00008194(0x9c,0x34,0x126,0x57,0);
    uVar3 = 0x89;
    uVar7 = 0x66;
    goto LAB_ram_00016d04;
  case '\x05':
    if (display_layout == '\0') {
      display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
      display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
      FUN_ram_0001a336();
      iVar4 = settings_language_access(0,0);
      if (iVar4 != 0) {
        FUN_ram_00008588(0xa6,0x16,"Voltagge:");
        FUN_ram_00008588(0xa6,0x57,"Current:",0xffff,0xffff,0x10,1);
        FUN_ram_00008588(0xa6,0x97,"Temperature:",0xffff,0xffff,0x10,1);
        goto LAB_ram_00016e88;
      }
      FUN_ram_00008e46(0xa6,0x16,&DAT_ram_00024974,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0xa6,0x57,&DAT_ram_0002497c,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0xa6,0x97,&DAT_ram_00024984,0xffff,0xffff,0x10,1);
      FUN_ram_000083d6(0xc9,0x17,0x3a,0x10,0xffff,0x1f,1);
      FUN_ram_000083d6(0xc9,0x56,0x3a,0x10,0xffff,0xffff,1);
      uVar3 = 0x98;
      uVar7 = 0xc9;
    }
    else {
      iVar4 = settings_language_access(0,0);
      if (iVar4 != 0) {
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,
                         "mode_set.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0,0);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"value.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0,0x28);
        resource_draw_jpeg("menu/value1.jpg",0x83,0);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"voltage.jpg"
                        );
        resource_draw_jpeg(&DAT_ram_20003304,0x88,7);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,
                         "electric.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0x88,0x54);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"tep.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0x88,0xa2);
        goto LAB_ram_00016e88;
      }
      display_fill_rectangle(0x83,0,0x140,0xf0,0);
      FUN_ram_0001a336();
      FUN_ram_00008e46(0x89,5,&DAT_ram_00024974,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0x89,0x52,&DAT_ram_0002497c,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0x89,0xa0,&DAT_ram_00024984,0xffff,0xffff,0x10,1);
      FUN_ram_000083d6(0xac,5,0x3a,0x10,0xffff,0x1f,1);
      FUN_ram_000083d6(0xac,0x52,0x3a,0x10,0xffff,0xffff,1);
      uVar3 = 0xa0;
      uVar7 = 0xac;
    }
    FUN_ram_000083d6(uVar7,uVar3,0x3a,0x10,0xffff,0xffff,1);
LAB_ram_00016e88:
    FUN_ram_000126d8();
    return '\0';
  case '\x06':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,2);
      uVar3 = 1;
LAB_ram_000170de:
      FUN_ram_0001a124(1,uVar3);
      return cVar1;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,2);
      uVar3 = 1;
LAB_ram_0001710e:
      FUN_ram_00019f20(1,uVar3);
      return '\0';
    }
    FUN_ram_00010e86(0);
    uVar3 = 1;
    break;
  case '\a':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,1);
      FUN_ram_0001a124(0,3);
      uVar3 = 2;
      goto LAB_ram_000170de;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,1);
      FUN_ram_00019f20(0,3);
      uVar3 = 2;
      goto LAB_ram_0001710e;
    }
    FUN_ram_00010e86(0);
    FUN_ram_00010e86(0,3);
    uVar3 = 2;
    break;
  case '\b':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,2);
      FUN_ram_0001a124(0,4);
      uVar3 = 3;
      goto LAB_ram_000170de;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,2);
      FUN_ram_00019f20(0,4);
      uVar3 = 3;
      goto LAB_ram_0001710e;
    }
    FUN_ram_00010e86(0);
    FUN_ram_00010e86(0,4);
    uVar3 = 3;
    break;
  case '\t':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,3);
      FUN_ram_0001a124(0,5);
      uVar3 = 4;
      goto LAB_ram_000170de;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,3);
      FUN_ram_00019f20(0,5);
      uVar3 = 4;
      goto LAB_ram_0001710e;
    }
    FUN_ram_00010e86(0);
    FUN_ram_00010e86(0,5);
    uVar3 = 4;
    break;
  case '\n':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,4);
      FUN_ram_0001a124(0,6);
      uVar3 = 5;
      goto LAB_ram_000170de;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,4);
      FUN_ram_00019f20(0,6);
      uVar3 = 5;
      goto LAB_ram_0001710e;
    }
    FUN_ram_00010e86(0);
    FUN_ram_00010e86(0,6);
    uVar3 = 5;
    break;
  case '\v':
    if (display_layout == '\0') {
      FUN_ram_0001a124(0,5);
      uVar3 = 6;
      goto LAB_ram_000170de;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00019f20(0,5);
      uVar3 = 6;
      goto LAB_ram_0001710e;
    }
    FUN_ram_00010e86(0);
    uVar3 = 6;
    break;
  case '\f':
    if ((display_layout != '\0') && (iVar4 = settings_language_access(0,0), iVar4 != 0)) {
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode_set.jpg")
      ;
      resource_draw_jpeg(&DAT_ram_20003304,0,0);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"history.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0,0x50);
      display_fill_rectangle(0x83,0,0x140,0xf0,0);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"history_.jpg")
      ;
      resource_draw_jpeg(&DAT_ram_20003304,0x83,0xf);
    }
    bVar5 = history_count;
    if (5 < history_count) {
      bVar5 = 5;
    }
    FUN_ram_00014578(1,bVar5,1);
    if (display_layout != '\0') {
      display_fill_rectangle(0x13b,5,0x13d,0xea,0);
      return '\0';
    }
    display_fill_rectangle(0x13d,0xb,0x13f,0xe4,0);
    return '\0';
  case '\r':
    if (display_layout == '\0') {
      DAT_ram_200001da = DAT_ram_200001d8;
      return '\0';
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      return '\0';
    }
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode_set.jpg");
    resource_draw_jpeg(&DAT_ram_20003304,0,0);
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"install.jpg");
    uVar3 = 0x78;
    uVar7 = 0;
    goto LAB_ram_00016b8a;
  case '\x0e':
    if (display_layout == '\0') {
      FUN_ram_00008194(0xeb,0x2f,0x120,0x5b,0);
      FUN_ram_00008194(0x9b,0x87,0x11d,0xa3,0x1c9f);
      uVar3 = 0;
LAB_ram_000174ec:
      uVar7 = 0xcb;
      uVar6 = 0xaf;
LAB_ram_0001746c:
      uVar9 = 0x11d;
      uVar2 = 0x9b;
      goto LAB_ram_00016cc0;
    }
    FUN_ram_00008194(0x8d,0x9a,0x135,0xbb,0xfd40);
    uVar3 = 0x6f;
    uVar7 = 0x4e;
    uVar9 = 0x8d;
    goto LAB_ram_00017526;
  case '\x0f':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9b,0x87,0x11d,0xa3,0);
      uVar3 = 0x1c9f;
      goto LAB_ram_000174ec;
    }
    goto LAB_ram_0001755e;
  default:
    goto switchD_ram_00016a4a_caseD_10;
  case '\x14':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9b,0x2f,0xd0,0x5b,0x1c9f);
      uVar3 = 0;
      uVar7 = 0x5b;
      uVar9 = 0x120;
      uVar6 = 0x2f;
      uVar2 = 0xeb;
      goto LAB_ram_00016cc0;
    }
    goto LAB_ram_000173f8;
  case '\x15':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9b,0x2f,0xd0,0x5b,0);
      FUN_ram_00008194(0xeb,0x2f,0x120,0x5b,0x1c9f);
      uVar3 = 0;
      uVar7 = 0xa3;
      uVar6 = 0x87;
      goto LAB_ram_0001746c;
    }
    FUN_ram_00008194(0x8d,0x4e,0x135,0x6f,0xfd40);
    uVar6 = 0;
    uVar3 = 0x43;
    uVar7 = 0x22;
    goto LAB_ram_0001749c;
  case '\x16':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x90,0x31,0x125,0x4b,0x1c9f);
      uVar3 = 0;
      uVar7 = 0xa4;
      uVar6 = 0x87;
LAB_ram_000175a0:
      uVar9 = 0x11b;
      uVar2 = 0x97;
      goto LAB_ram_00016cc0;
    }
    uVar6 = 0xfd40;
    uVar3 = 0x53;
    uVar7 = 0x32;
    uVar9 = 0x88;
    goto LAB_ram_000174a0;
  case '\x17':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x90,0x31,0x125,0x4b,0);
      FUN_ram_00008194(0x97,0x87,0x11b,0xa4,0x1c9f);
      uVar3 = 0;
LAB_ram_000175f8:
      uVar7 = 0xca;
      uVar6 = 0xad;
      goto LAB_ram_000175a0;
    }
    FUN_ram_00008194(0x8d,0x9a,0x135,0xbb,0xfd40);
    uVar3 = 0x53;
    uVar7 = 0x32;
    uVar9 = 0x88;
LAB_ram_00017526:
    FUN_ram_00008194(uVar9,uVar7,0x135,uVar3,0);
    uVar3 = 0xe7;
    uVar7 = 0xc6;
LAB_ram_00017420:
    uVar6 = 0x135;
    uVar9 = 0x8d;
    goto LAB_ram_00016d08;
  case '\x18':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x97,0x87,0x11b,0xa4,0);
      uVar3 = 0x1c9f;
      goto LAB_ram_000175f8;
    }
LAB_ram_0001755e:
    uVar6 = 0xfd40;
    uVar3 = 0xe7;
    uVar7 = 0xc6;
LAB_ram_0001749c:
    uVar9 = 0x8d;
LAB_ram_000174a0:
    FUN_ram_00008194(uVar9,uVar7,0x135,uVar3,uVar6);
    uVar3 = 0xbb;
    uVar7 = 0x9a;
    goto LAB_ram_00017420;
  case '\x19':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x46,0x11d,0x62,0x1c9f);
      uVar3 = 0;
      uVar7 = 0x81;
      uVar9 = 0x11d;
      uVar6 = 0x65;
      goto LAB_ram_00016cbc;
    }
LAB_ram_000173f8:
    FUN_ram_00008194(0x8d,0x22,0x135,0x43,0xfd40);
    uVar3 = 0x6f;
    uVar7 = 0x4e;
    goto LAB_ram_00017420;
  case '\x1a':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x46,0x11d,0x62,0);
      FUN_ram_00008194(0x9c,0x65,0x11d,0x81,0x1c9f);
      uVar3 = 0;
      uVar7 = 0xa0;
      uVar9 = 0x11d;
      uVar6 = 0x84;
      goto LAB_ram_00016cbc;
    }
    FUN_ram_00008194(0x8d,0x4e,0x135,0x6f,0xfd40);
    uVar9 = 0;
    uVar3 = 0x43;
    uVar7 = 0x22;
    goto LAB_ram_000176fc;
  case '\x1b':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x65,0x11d,0x81,0);
      FUN_ram_00008194(0x9c,0x84,0x11d,0xa0,0x1c9f);
      uVar3 = 0;
LAB_ram_0001774c:
      uVar7 = 0xbf;
      uVar9 = 0x11d;
      uVar6 = 0xa3;
      goto LAB_ram_00016cbc;
    }
    FUN_ram_00008194(0x8d,0x7a,0x135,0x9b,0xfd40);
    FUN_ram_00008194(0x8d,0x4e,0x135,0x6f,0);
    uVar3 = 199;
    uVar7 = 0xa6;
    goto LAB_ram_00017420;
  case '\x1c':
    if (display_layout == '\0') {
      FUN_ram_00008194(0x9c,0x84,0x11d,0xa0,0);
      uVar3 = 0x1c9f;
      goto LAB_ram_0001774c;
    }
    uVar9 = 0xfd40;
    uVar3 = 199;
    uVar7 = 0xa6;
LAB_ram_000176fc:
    FUN_ram_00008194(0x8d,uVar7,0x135,uVar3,uVar9);
    uVar3 = 0x9b;
    uVar7 = 0x7a;
    goto LAB_ram_00017420;
  case '\x1d':
    if (display_layout == '\0') {
      FUN_ram_000081ce(0x8a,0x2b,0x130,0x91,10,0x4228);
      iVar4 = settings_language_access(0,0);
      if (iVar4 == 0) {
        FUN_ram_00008c64(0xb8,0x36,&PTR_LAB_ram_0000f8a0_1_ram_000249e8,0xfd40,0xfd40,0x18,1);
        FUN_ram_000083d6(0xc0,0x3b,0x21,0x10,0,0,1);
        FUN_ram_00008e46(0xd0,0x36,&DAT_ram_00024a0c,0xfd40,0xfd40,0x18,1);
        FUN_ram_00008e46(0x9d,0x57,&DAT_ram_00024a14,0xffff,0xffff,0x10,1);
        FUN_ram_00008e46(0x9d,0x70,&DAT_ram_00024a28,0xffff,0xffff,0x10,1);
      }
      else {
        FUN_ram_00008c64(0xac,0x36,&PTR_LAB_ram_0000f8a0_1_ram_000249e8,0xfd40,0xfd40,0x18,1);
        FUN_ram_000083d6(0xb4,0x3b,0x21,0x10,0,0,1);
        FUN_ram_00008588(0xc9,0x36,"Notice",0xfd40,0xfd40,0x18,1);
        FUN_ram_00008588(0xbd,0x57,"Reset all",0xffff,0xffff,0x10,1);
        FUN_ram_00008588(0xb6,0x70,"settings!!!",0xffff,0xffff,0x10,1);
      }
      uVar3 = 0;
LAB_ram_00017896:
      FUN_ram_000152a8(uVar3);
      return cVar1;
    }
    iVar4 = settings_language_access(0,0);
    if (iVar4 != 0) {
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"mode_set.jpg")
      ;
      resource_draw_jpeg(&DAT_ram_20003304,0,0);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"recover.jpg");
      resource_draw_jpeg(&DAT_ram_20003304,0,0xa0);
    }
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"recover1.jpg");
    uVar3 = 0;
    uVar7 = 0x83;
    goto LAB_ram_00016b8a;
  case '\x1e':
    uVar3 = 1;
    if (display_layout == '\0') goto LAB_ram_00017896;
    pcVar8 = "recover1_1.jpg";
    goto LAB_ram_000179ae;
  case '\x1f':
    if (display_layout == '\0') {
      uVar3 = 2;
      goto LAB_ram_00017896;
    }
    pcVar8 = "recover1_2.jpg";
LAB_ram_000179ae:
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,pcVar8);
    uVar3 = 0x93;
    uVar7 = 0x8f;
LAB_ram_00016b8a:
    resource_draw_jpeg(&DAT_ram_20003304,uVar7,uVar3);
    return '\0';
  case ' ':
    if (display_layout == '\0') {
      display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
      display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
    }
    else {
      display_fill_rectangle(0x83,0,0x140,0xf0,0);
      iVar4 = settings_language_access(0,0);
      if (iVar4 != 0) {
        resource_draw_jpeg("menu/mode_set.jpg",0,0);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,
                         "mode_set.jpg");
        resource_draw_jpeg(&DAT_ram_20003304,0,0);
        FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"message.jpg"
                        );
        resource_draw_jpeg(&DAT_ram_20003304,0,200);
      }
    }
    resource_draw_jpeg("menu/about.jpg",0x83,0x32);
    FUN_ram_000143e0();
    goto switchD_ram_00016a4a_caseD_10;
  }
  FUN_ram_00010e86(1,uVar3);
switchD_ram_00016a4a_caseD_10:
  return '\0';
}

