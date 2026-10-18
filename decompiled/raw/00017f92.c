/* Original address: 0x00017f92; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_00017f92(void)

{
  int iVar1;
  undefined4 uVar2;
  char *pcVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  iVar1 = 0xf8;
  if (display_layout == '\0') {
    iVar1 = 0xfb;
  }
  uVar2 = FUN_ram_0000a7a4();
  switch(measurement_mode + -10) {
  case '\0':
    if (measurement_unit_code == '\x10') {
      if (DAT_ram_200001da == '\x10') {
        return 0;
      }
      DAT_ram_200001da = measurement_unit_code;
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/uf.jpg";
    }
    else if (measurement_unit_code == '\x11') {
      if (DAT_ram_200001da == '\x12') {
        return 0;
      }
      DAT_ram_200001da = '\x12';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/mf.jpg";
    }
    else {
      if (measurement_unit_code != '\x0f') {
        return uVar2;
      }
      if (DAT_ram_200001da == '\x11') {
        return 0;
      }
      DAT_ram_200001da = '\x11';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/nf.jpg";
    }
    break;
  case '\x01':
  case '\x02':
  case '\x04':
  case '\n':
    switch(measurement_unit_code + -6) {
    case '\0':
      if (DAT_ram_200001da == '\a') {
        return 0;
      }
      DAT_ram_200001da = '\a';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/v.jpg";
      break;
    case '\x01':
      if (DAT_ram_200001da == '\b') {
        return 0;
      }
      DAT_ram_200001da = '\b';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/mv.jpg";
      break;
    default:
      uVar2 = display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      DAT_ram_200001da = 0x19;
      return uVar2;
    case '\x05':
      if (DAT_ram_200001da == '\x0e') {
        return 0;
      }
      DAT_ram_200001da = '\x0e';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/r.jpg";
      break;
    case '\x06':
      if (DAT_ram_200001da == '\r') {
        return 0;
      }
      DAT_ram_200001da = '\r';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/kr.jpg";
      break;
    case '\a':
      if (DAT_ram_200001da == '\f') {
        return 0;
      }
      DAT_ram_200001da = '\f';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/mr.jpg";
    }
    break;
  default:
    goto switchD_ram_00014448_default;
  case '\x05':
    if (measurement_unit_code == '\x13') {
      if (DAT_ram_200001da == '\x14') {
        return 0;
      }
      DAT_ram_200001da = '\x14';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/khz.jpg";
    }
    else if (measurement_unit_code == '\x14') {
      if (DAT_ram_200001da == '\x13') {
        return 0;
      }
      DAT_ram_200001da = '\x13';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/mhz.jpg";
    }
    else {
      if (measurement_unit_code != '\x12') {
        return uVar2;
      }
      if (DAT_ram_200001da == '\x15') {
        return 0;
      }
      DAT_ram_200001da = '\x15';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/hz.jpg";
    }
    break;
  case '\x06':
  case '\a':
  case '\v':
  case '\f':
    if (measurement_unit_code == '\t') {
      if (DAT_ram_200001da == '\n') {
        return 0;
      }
      DAT_ram_200001da = '\n';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/ma.jpg";
    }
    else if (measurement_unit_code == '\n') {
      if (DAT_ram_200001da == '\t') {
        return 0;
      }
      DAT_ram_200001da = '\t';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/a.jpg";
    }
    else {
      if (measurement_unit_code != '\b') {
        return uVar2;
      }
      if (DAT_ram_200001da == '\v') {
        return 0;
      }
      DAT_ram_200001da = '\v';
      display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
      pcVar3 = "units/ua.jpg";
    }
    break;
  case '\b':
    if (measurement_unit_code != '\x15') {
      return uVar2;
    }
    if (DAT_ram_200001da == '\x16') {
      return 0;
    }
    DAT_ram_200001da = '\x16';
    display_fill_rectangle(iVar1,0x80,iVar1 + 0x23,0xa3,0);
    uVar4 = settings_temperature_access(0,0);
    if (uVar4 == 0) {
      pcVar3 = "units/c.jpg";
    }
    else {
      pcVar3 = "units/f.jpg";
    }
  }
  uVar2 = resource_draw_jpeg(pcVar3,iVar1,0x80);
switchD_ram_00014448_default:
  return uVar2;
}

