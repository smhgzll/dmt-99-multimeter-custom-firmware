/* Original address: 0x0000a910; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: 0xfd event dispatch and resource paths for measurement modes */

void measurement_ui_regular_event(void)

{
  uint8_t uVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  int iVar6;
  
  uVar1 = measurement_dma_frame.packed_segments[0];
  gp = &__global_pointer;
  iVar2 = 0;
  if (display_layout == '\0') {
    iVar2 = 0x74;
  }
  if (measurement_dma_frame.status_or_segments0 != 0xfd) {
    if (measurement_dma_frame.first != 'Z') {
      return;
    }
    FUN_ram_00017f92();
    if (measurement_mode != '\x14') {
      return;
    }
    FUN_ram_0000a7d0();
    return;
  }
  if ((measurement_mode == '\x13') || (measurement_mode == '\t')) {
    if (measurement_dma_frame.packed_segments[0] != '\x13') {
      display_fill(0);
      FUN_ram_00014ef2();
      goto LAB_ram_0000a99a;
    }
LAB_ram_0000a958:
    measurement_dma_frame.packed_segments[0] = uVar1;
    if (DAT_ram_2000023e == '\x01') {
      if (display_layout == '\0') {
        FUN_ram_0001a360(&DAT_ram_00026338,0x51,0xca,0,0x4228);
      }
      else {
        FUN_ram_0001aede("LCD_Fill");
        display_fill_rectangle(0x43,0x40,0x73,0x4f,0);
      }
      DAT_ram_2000023e = '\0';
    }
  }
  else {
LAB_ram_0000a99a:
    measurement_dma_frame.packed_segments[0] = uVar1;
    if (uVar1 != '\x03') goto LAB_ram_0000a958;
  }
  uVar1 = measurement_dma_frame.packed_segments[0];
  switch(measurement_dma_frame.packed_segments[0]) {
  case '\x03':
    if (DAT_ram_2000023e != '\0') {
      if (display_layout == '\0') {
        FUN_ram_0001a360(&DAT_ram_00026338,0x51,0xca,0,0x4228);
      }
      else {
        display_fill_rectangle(0x3e,0x40,0x78,0x4f,0);
      }
      DAT_ram_2000023e = 0;
      return;
    }
    DAT_ram_2000023e = '\x01';
    if (display_layout == '\0') {
      iVar6 = 0xca;
      uVar5 = 0x51;
    }
    else {
      iVar6 = 0x40;
      uVar5 = 0x43;
    }
    pcVar4 = "mode/hold.jpg";
    break;
  default:
    goto LAB_ram_0000664a;
  case '\t':
    measurement_mode = 9;
    return;
  case '\n':
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    measurement_mode = '\n';
    iVar3 = settings_language_access(0,0);
    iVar6 = 0x96 - iVar2;
    if (iVar3 == 0) {
      uVar5 = 0x6b;
      pcVar4 = "mode/cap_c.jpg";
    }
    else {
      uVar5 = 0x77;
      pcVar4 = "mode/cap.jpg";
    }
    break;
  case '\v':
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    measurement_mode = '\v';
    iVar3 = settings_language_access(0,0);
    iVar6 = 0x96 - iVar2;
    if (iVar3 == 0) {
      uVar5 = 0x6b;
      pcVar4 = "mode/r_c.jpg";
    }
    else {
      uVar5 = 0x77;
      pcVar4 = "mode/r.jpg";
    }
    break;
  case '\f':
  case '\r':
  case '\x10':
  case '\x11':
  case '\x15':
  case '\x16':
    if (measurement_dma_frame.packed_segments[0] == '\f') {
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        uVar5 = 0x6b;
        pcVar4 = "mode/dc_v_c.jpg";
      }
      else {
        uVar5 = 0x77;
        pcVar4 = "mode/dc_v.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\r') {
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        uVar5 = 0x6b;
        pcVar4 = "mode/ac_v_c.jpg";
      }
      else {
        uVar5 = 0x77;
        pcVar4 = "mode/ac_v.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x10') {
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        pcVar4 = "mode/dc_a_c.jpg";
LAB_ram_0000ab06:
        resource_draw_jpeg(pcVar4,0x5d,iVar6);
        iVar6 = 0x9c - iVar2;
        uVar5 = 0xc5;
        pcVar4 = "units/a.jpg";
      }
      else {
        uVar5 = 0x77;
        pcVar4 = "mode/dc_a.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x11') {
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        pcVar4 = "mode/dc_a_c.jpg";
LAB_ram_0000ab7c:
        resource_draw_jpeg(pcVar4,0x5d,iVar6);
        iVar6 = 0x9c - iVar2;
        uVar5 = 0xc5;
        pcVar4 = "units/ma.jpg";
      }
      else {
        uVar5 = 0x77;
        pcVar4 = "mode/dc_a-.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x15') {
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        pcVar4 = "mode/ac_a_c.jpg";
        goto LAB_ram_0000ab06;
      }
      uVar5 = 0x7e;
      pcVar4 = "mode/ac_a.jpg";
    }
    else {
      if (measurement_dma_frame.packed_segments[0] != '\x16') {
        return;
      }
      display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
      measurement_mode = uVar1;
      iVar3 = settings_language_access(0,0);
      iVar6 = 0x96 - iVar2;
      if (iVar3 == 0) {
        pcVar4 = "mode/ac_a_c.jpg";
        goto LAB_ram_0000ab7c;
      }
      uVar5 = 0x7e;
      pcVar4 = "mode/ac_a-.jpg";
    }
    break;
  case '\x0e':
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    measurement_mode = '\x0e';
    iVar3 = settings_language_access(0,0);
    iVar6 = 0x96 - iVar2;
    if (iVar3 == 0) {
      uVar5 = 0x6b;
      pcVar4 = "mode/ol_c.jpg";
    }
    else {
      uVar5 = 0x77;
      pcVar4 = "mode/ol.jpg";
    }
    break;
  case '\x0f':
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    measurement_mode = '\x0f';
    iVar3 = settings_language_access(0,0);
    iVar6 = 0x96 - iVar2;
    if (iVar3 == 0) {
      uVar5 = 0x6b;
      pcVar4 = "mode/hz_c.jpg";
    }
    else {
      uVar5 = 0x77;
      pcVar4 = "mode/hz.jpg";
    }
    break;
  case '\x12':
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    measurement_mode = '\x12';
    iVar3 = settings_language_access(0,0);
    iVar6 = 0x96 - iVar2;
    if (iVar3 == 0) {
      uVar5 = 0x6b;
      pcVar4 = "mode/temp_c.jpg";
    }
    else {
      uVar5 = 0x77;
      pcVar4 = "mode/temp.jpg";
    }
    break;
  case '\x13':
    measurement_mode = 0x13;
    return;
  case '\x14':
    measurement_mode = '\x14';
    display_fill_rectangle(100,0x96U - iVar2 & 0xffff,0xe1,0xafU - iVar2 & 0xffff,0);
    goto LAB_ram_0000664a;
  }
  resource_draw_jpeg(pcVar4,uVar5,iVar6);
LAB_ram_0000664a:
  return;
}

