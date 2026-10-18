/* Original address: 0x00018354; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_00018354(void)

{
  int iVar1;
  undefined4 uVar2;
  char *pcVar3;
  uint uVar4;
  undefined6 uVar5;
  undefined1 auStack_18 [8];
  
  gp = &__global_pointer;
  uVar2 = FUN_ram_0000a7a4();
  iVar1 = 0;
  if (display_layout == '\0') {
    iVar1 = 0x28;
  }
  switch(measurement_mode + -10) {
  case '\0':
    if (measurement_unit_code == '\x10') {
      if (DAT_ram_200001da != '\x10') {
        DAT_ram_200001da = measurement_unit_code;
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/uf.jpg";
        goto LAB_ram_000184da;
      }
    }
    else if (measurement_unit_code == '\x11') {
      if (DAT_ram_200001da != '\x12') {
        DAT_ram_200001da = 0x12;
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        resource_draw_jpeg("units/mf.jpg",0xaa - iVar1,0x1e);
        uVar2 = FUN_ram_0000969e(auStack_18,"%c%c%c",0x6d,0x46,0x20,0);
        return uVar2;
      }
    }
    else {
      if (measurement_unit_code != '\x0f') {
        return uVar2;
      }
      if (DAT_ram_200001da != '\x11') {
        DAT_ram_200001da = '\x11';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/nf.jpg";
        goto LAB_ram_000184da;
      }
    }
    break;
  case '\x01':
  case '\x02':
  case '\x03':
    switch(measurement_unit_code + -6) {
    case '\0':
      if (DAT_ram_200001da != '\a') {
        DAT_ram_200001da = 7;
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        resource_draw_jpeg("units/v.jpg",0xaa - iVar1,0x1e);
        display_fill_rectangle(0xd1,0xdb,0xee,0xea,0);
        display_fill_rectangle(0x6b,0xdb,0x88,0xea,0);
        uVar2 = 0x56;
        pcVar3 = "%c%c%c%c";
LAB_ram_00018434:
        FUN_ram_0000969e(auStack_18,pcVar3,uVar2,0x20,0x20,0);
LAB_ram_0001843a:
        FUN_ram_000085cc(0xd1,0xdb,0x14,0x3c,0x10,0xffff,auStack_18);
        uVar2 = FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_18);
        return uVar2;
      }
      break;
    case '\x01':
switchD_ram_00018494_caseD_1:
      if (DAT_ram_200001da != '\b') {
        DAT_ram_200001da = '\b';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/mv.jpg";
LAB_ram_000184da:
        uVar2 = resource_draw_jpeg(pcVar3,iVar1,0x1e);
        return uVar2;
      }
      break;
    default:
      goto switchD_ram_00018388_caseD_9;
    case '\x05':
switchD_ram_00018494_caseD_5:
      if (DAT_ram_200001da != '\x0e') {
        DAT_ram_200001da = '\x0e';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/r.jpg";
        goto LAB_ram_000184da;
      }
      break;
    case '\x06':
switchD_ram_00018494_caseD_6:
      if (DAT_ram_200001da != '\r') {
        DAT_ram_200001da = '\r';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/kr.jpg";
        goto LAB_ram_000184da;
      }
      break;
    case '\a':
switchD_ram_00018494_caseD_7:
      if (DAT_ram_200001da != '\f') {
        DAT_ram_200001da = '\f';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/mr.jpg";
        goto LAB_ram_000184da;
      }
    }
    break;
  case '\x04':
    switch(measurement_unit_code + -6) {
    case '\0':
      if (DAT_ram_200001da != '\x06') {
        DAT_ram_200001da = '\x06';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/v.jpg";
        goto LAB_ram_000184da;
      }
      break;
    case '\x01':
      goto switchD_ram_00018494_caseD_1;
    default:
      goto switchD_ram_00018388_caseD_9;
    case '\x05':
      goto switchD_ram_00018494_caseD_5;
    case '\x06':
      goto switchD_ram_00018494_caseD_6;
    case '\a':
      goto switchD_ram_00018494_caseD_7;
    }
    break;
  case '\x05':
    if (measurement_unit_code == '\x13') {
      if (DAT_ram_200001da != '\x14') {
        DAT_ram_200001da = '\x14';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/khz.jpg";
        goto LAB_ram_000184da;
      }
    }
    else if (measurement_unit_code == '\x14') {
      if (DAT_ram_200001da != '\x13') {
        DAT_ram_200001da = '\x13';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/mhz.jpg";
        goto LAB_ram_000184da;
      }
    }
    else {
      if (measurement_unit_code != '\x12') {
        return uVar2;
      }
      if (DAT_ram_200001da != '\x15') {
        DAT_ram_200001da = '\x15';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/hz.jpg";
        goto LAB_ram_000184da;
      }
    }
    break;
  case '\x06':
  case '\a':
  case '\v':
  case '\f':
    if (measurement_unit_code == '\t') {
      if (DAT_ram_200001da != '\n') {
        DAT_ram_200001da = '\n';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/ma.jpg";
        goto LAB_ram_000184da;
      }
    }
    else if (measurement_unit_code == '\n') {
      if (DAT_ram_200001da != '\t') {
        DAT_ram_200001da = 9;
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        resource_draw_jpeg("units/a.jpg",0xaa - iVar1,0x1e);
        uVar2 = 0x41;
        pcVar3 = "%c%c%c";
        goto LAB_ram_00018434;
      }
    }
    else {
      if (measurement_unit_code != '\b') {
        return uVar2;
      }
      if (DAT_ram_200001da != '\v') {
        DAT_ram_200001da = '\v';
        display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
        iVar1 = 0xaa - iVar1;
        pcVar3 = "units/ua.jpg";
        goto LAB_ram_000184da;
      }
    }
    break;
  case '\b':
    if (measurement_unit_code != '\x15') {
      return uVar2;
    }
    if (DAT_ram_200001da != '\x16') {
      DAT_ram_200001da = 0x16;
      display_fill_rectangle(0xaaU - iVar1 & 0xffff,0x1e,0xcdU - iVar1 & 0xffff,0x31,0);
      uVar4 = settings_temperature_access(0,0);
      if (uVar4 == 0) {
        pcVar3 = "units/c.jpg";
      }
      else {
        pcVar3 = "units/f.jpg";
      }
      uVar5 = resource_draw_jpeg(pcVar3,0xaa - iVar1,0x1e);
      uVar4 = settings_temperature_access((uint)uVar5,(short)((uint6)uVar5 >> 0x20));
      uVar2 = 0x5a;
      if (uVar4 != 0) {
        uVar2 = 0x4a;
      }
      FUN_ram_0000969e(auStack_18,"%c%c%c",uVar2,0x20,0);
      goto LAB_ram_0001843a;
    }
    break;
  default:
    goto switchD_ram_00018388_caseD_9;
  }
  uVar2 = 0;
switchD_ram_00018388_caseD_9:
  return uVar2;
}

