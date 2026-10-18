/* Semantic review: high. 0xfd event dispatch in second application branch.
 * Address 0x0000aed0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000aed0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: 0xfd event dispatch in second application branch */

void measurement_ui_record_event(void)

{
  int iVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  if (measurement_dma_frame.status_or_segments0 != 0xfd) {
    FUN_ram_00018354();
    return;
  }
  if ((measurement_dma_frame.packed_segments[0] != '\x03') && (DAT_ram_2000023e == '\x01')) {
    display_fill_rectangle(0xff,0x19,0x12f,0x28,0);
    DAT_ram_2000023e = '\0';
  }
  switch(measurement_dma_frame.packed_segments[0]) {
  case '\x03':
    if (DAT_ram_2000023e == '\0') {
      resource_draw_jpeg("mode/hold.jpg",0xff,0x19);
      DAT_ram_2000023e = 1;
      return;
    }
    display_fill_rectangle(0xff,0x19,0x12f,0x28,0);
    DAT_ram_2000023e = 0;
    return;
  default:
    goto switchD_ram_00014448_default;
  case '\n':
    measurement_mode = '\n';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      display_fill_rectangle(5,0xb2,0x23,0xbf,0);
      uVar4 = 0x44;
      uVar3 = 8;
      pcVar2 = "mode/mode2_cap_c.jpg";
    }
    else {
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_cap.jpg";
    }
    break;
  case '\v':
    measurement_mode = '\v';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      display_fill_rectangle(5,0xb2,0x23,0xbf,0);
      uVar4 = 0x44;
      uVar3 = 8;
      pcVar2 = "mode/mode2_r_c.jpg";
    }
    else {
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_r.jpg";
    }
    break;
  case '\f':
  case '\r':
  case '\x10':
  case '\x11':
  case '\x15':
  case '\x16':
    if (measurement_dma_frame.packed_segments[0] == '\f') {
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        uVar4 = 0x44;
        uVar3 = 8;
        pcVar2 = "mode/mode2_dc_v_c.jpg";
      }
      else {
        uVar4 = 0x71;
        uVar3 = 0;
        pcVar2 = "mode/mode2_ac_v.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\r') {
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        uVar4 = 0x44;
        uVar3 = 8;
        pcVar2 = "mode/mode2_ac_v_c.jpg";
      }
      else {
        uVar4 = 0x71;
        uVar3 = 0;
        pcVar2 = "mode/mode2_dc_v.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x10') {
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        pcVar2 = "mode/mode2_dc_a_c.jpg";
LAB_ram_0000b00c:
        resource_draw_jpeg(pcVar2,8,0x44);
        uVar4 = 0xb2;
        uVar3 = 0xc;
        pcVar2 = "units/a.jpg";
      }
      else {
        uVar4 = 0x71;
        uVar3 = 0;
        pcVar2 = "mode/mode2_dc_a.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x11') {
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        pcVar2 = "mode/mode2_dc_a_c.jpg";
LAB_ram_0000b066:
        resource_draw_jpeg(pcVar2,8,0x44);
        uVar4 = 0xb2;
        uVar3 = 5;
        pcVar2 = "units/ma.jpg";
      }
      else {
        uVar4 = 0x71;
        uVar3 = 0;
        pcVar2 = "mode/mode2_dc_ma.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x15') {
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        pcVar2 = "mode/mode2_ac_a_c.jpg";
        goto LAB_ram_0000b00c;
      }
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_ac_a.jpg";
    }
    else {
      if (measurement_dma_frame.packed_segments[0] != '\x16') {
        return;
      }
      measurement_mode = measurement_dma_frame.packed_segments[0];
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        display_fill_rectangle(5,0xb2,0x23,0xbf,0);
        pcVar2 = "mode/mode2_ac_a_c.jpg";
        goto LAB_ram_0000b066;
      }
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_ac_a-.jpg";
    }
    break;
  case '\x0e':
    measurement_mode = '\x0e';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      display_fill_rectangle(5,0xb2,0x23,0xbf,0);
      uVar4 = 0x44;
      uVar3 = 8;
      pcVar2 = "mode/mode2_ol_c.jpg";
    }
    else {
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_ol.jpg";
    }
    break;
  case '\x0f':
    measurement_mode = '\x0f';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      display_fill_rectangle(5,0xb2,0x23,0xbf,0);
      uVar4 = 0x44;
      uVar3 = 8;
      pcVar2 = "mode/mode2_hz_c.jpg";
    }
    else {
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_hz.jpg";
    }
    break;
  case '\x12':
    measurement_mode = '\x12';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      display_fill_rectangle(5,0xb2,0x23,0xbf,0);
      uVar4 = 0x44;
      uVar3 = 8;
      pcVar2 = "mode/mode2_tem_c.jpg";
    }
    else {
      uVar4 = 0x71;
      uVar3 = 0;
      pcVar2 = "mode/mode2_tem.jpg";
    }
  }
  resource_draw_jpeg(pcVar2,uVar3,uVar4);
switchD_ram_00014448_default:
  return;
}

