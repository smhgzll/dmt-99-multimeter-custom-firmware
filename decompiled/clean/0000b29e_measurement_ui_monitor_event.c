/* Semantic review: medium. Third application branch event processing and threshold UI.
 * Address 0x0000b29e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000b29e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: Third application branch event processing and threshold UI */

void measurement_ui_monitor_event(void)

{
  int iVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  if (measurement_dma_frame.status_or_segments0 != 0xfd) {
    FUN_ram_00018974();
    return;
  }
  if ((measurement_dma_frame.packed_segments[0] != '\x03') && (DAT_ram_2000023e == '\x01')) {
    if (display_layout == '\0') {
      uVar4 = 199;
      uVar3 = 0xb8;
    }
    else {
      uVar4 = 0xbd;
      uVar3 = 0xae;
    }
    display_fill_rectangle(0x88,uVar3,0xb8,uVar4,0);
    DAT_ram_2000023e = '\0';
  }
  switch(measurement_dma_frame.packed_segments[0]) {
  case '\x03':
    if (DAT_ram_2000023e == '\0') {
      uVar4 = 0xb8;
      if (display_layout != '\0') {
        uVar4 = 0xae;
      }
      resource_draw_jpeg("mode/hold.jpg",0x88,uVar4);
      DAT_ram_2000023e = 1;
      return;
    }
    if (display_layout == '\0') {
      uVar4 = 199;
      uVar3 = 0xb8;
    }
    else {
      uVar4 = 0xbd;
      uVar3 = 0xae;
    }
    display_fill_rectangle(0x88,uVar3,0xb8,uVar4,0);
    DAT_ram_2000023e = 0;
    return;
  default:
    goto switchD_ram_00014448_default;
  case '\v':
    measurement_mode = '\v';
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      uVar4 = 0x6a;
      pcVar2 = "mode/r_c.jpg";
    }
    else {
      uVar4 = 0x77;
      pcVar2 = "mode/r.jpg";
    }
    break;
  case '\f':
  case '\r':
  case '\x10':
  case '\x15':
    if (measurement_dma_frame.packed_segments[0] == '\f') {
      if (((measurement_mode != '\f') && (measurement_mode != '\r')) || (DAT_ram_20000204 == '\0'))
      {
        DAT_ram_20000204 = '\x01';
        measurement_mode = 0xc;
        FUN_ram_00012d5a(0xc);
      }
      measurement_mode = '\f';
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar4 = 0x6a;
        pcVar2 = "mode/dc_v_c.jpg";
      }
      else {
        uVar4 = 0x77;
        pcVar2 = "mode/dc_v.jpg";
      }
      resource_draw_jpeg(pcVar2,uVar4,0x5a);
    }
    if (measurement_dma_frame.packed_segments[0] == '\r') {
      if (((measurement_mode != '\f') && (measurement_mode != '\r')) || (DAT_ram_20000204 == '\0'))
      {
        DAT_ram_20000204 = '\x01';
        measurement_mode = 0xd;
        FUN_ram_00012d5a(0xd);
      }
      measurement_mode = '\r';
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar4 = 0x6a;
        pcVar2 = "mode/ac_v_c.jpg";
      }
      else {
        uVar4 = 0x77;
        pcVar2 = "mode/ac_v.jpg";
      }
    }
    else if (measurement_dma_frame.packed_segments[0] == '\x10') {
      if (((measurement_mode != '\x10') && (measurement_mode != '\x15')) ||
         (DAT_ram_20000204 == '\0')) {
        DAT_ram_20000204 = '\x01';
        measurement_mode = 0x10;
        FUN_ram_00012d5a(0x10);
      }
      measurement_mode = '\x10';
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar4 = 0x6a;
        pcVar2 = "mode/dc_a_c.jpg";
      }
      else {
        uVar4 = 0x77;
        pcVar2 = "mode/dc_a.jpg";
      }
    }
    else {
      if (measurement_dma_frame.packed_segments[0] != '\x15') {
        return;
      }
      if (((measurement_mode != '\x10') && (measurement_mode != '\x15')) ||
         (DAT_ram_20000204 == '\0')) {
        DAT_ram_20000204 = '\x01';
        measurement_mode = 0x15;
        FUN_ram_00012d5a(0x15);
      }
      measurement_mode = '\x15';
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar4 = 0x6a;
        pcVar2 = "mode/ac_a_c.jpg";
      }
      else {
        uVar4 = 0x77;
        pcVar2 = "mode/ac_a.jpg";
      }
    }
    break;
  case '\x12':
    if (measurement_mode != '\x12') {
      measurement_mode = '\x12';
      FUN_ram_00012d5a(0x12);
    }
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      uVar4 = 0x6a;
      pcVar2 = "mode/temp_c.jpg";
    }
    else {
      uVar4 = 0x7d;
      pcVar2 = "mode/temp.jpg";
    }
  }
  resource_draw_jpeg(pcVar2,uVar4,0x5a);
switchD_ram_00014448_default:
  return;
}

