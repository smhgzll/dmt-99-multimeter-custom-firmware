/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00014ef2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00014ef2; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00014ef2(void)

{
  short sVar1;
  short sVar2;
  char *pcVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  gp = &__global_pointer;
  battery_percent = battery_percentage_estimate();
  if (application_view == '\x01') {
    if (DAT_ram_2000024c == '\0') {
      DAT_ram_2000024c = application_view;
    }
    if (display_layout == '\0') {
      resource_draw_jpeg("motif2/mode1_adorn1.jpg",0x30,0x38);
      resource_draw_jpeg("motif2/mode1_adorn2.jpg",0x30,0xa3);
      FUN_ram_0001a360(&DAT_ram_00026398,0xcd,0xca,0,0x4228);
      FUN_ram_0001a360(&DAT_ram_00026338,0x51,0xca,0,0x4228);
      goto LAB_ram_00014f80;
    }
    resource_draw_jpeg("cell_rin.jpg",0,0xd5);
    uVar6 = 0;
    uVar5 = 0x1e;
    pcVar3 = "gImage.jpg";
  }
  else {
    if (application_view == '\x02') {
      FUN_ram_00008194(0x24,0x34,0xf0,0xeb,0x4228);
      FUN_ram_00008194(0xf2,0x34,0x13e,0xeb,0x4228);
      iVar4 = settings_language_access(0,0);
      if (iVar4 == 0) {
        pcVar3 = "mode/mode2_max_c.jpg";
      }
      else {
        pcVar3 = "mode/mode2_max.jpg";
      }
      resource_draw_jpeg(pcVar3,0x24,0xda);
      sVar1 = 0x44;
      do {
        FUN_ram_00008050(0x104,sVar1,0x108,sVar1,0xffff);
        FUN_ram_00008050(0x10b,sVar1,0x10f,sVar1,0xffff);
        sVar2 = sVar1 + 0x15;
        FUN_ram_00008050(0x112,sVar1,0x116,sVar1,0xffff);
        sVar1 = sVar2;
      } while (sVar2 != 0xec);
      goto LAB_ram_00014f80;
    }
    if (display_layout == '\0') {
      uVar6 = 0xcf;
      uVar5 = 0;
      pcVar3 = "motif2/mode3_value.jpg";
    }
    else {
      resource_draw_jpeg("cell_rin.jpg",0,0xd5);
      FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,
                       "mode3_value.jpg");
      uVar6 = 0xbf;
      uVar5 = 9;
      pcVar3 = &DAT_ram_20003304;
    }
  }
  resource_draw_jpeg(pcVar3,uVar5,uVar6);
LAB_ram_00014f80:
  FUN_ram_00009caa();
  if (display_layout == '\0') {
    resource_draw_jpeg("motif2/electric_frame.jpg",1,0);
    FUN_ram_00014e48(battery_percent);
  }
  else {
    battery_icon_theme_a(battery_percent);
    resource_draw_jpeg("cell/-cell.jpg",10,2);
    resource_draw_jpeg("cell/--cell.jpg",2,2);
    resource_draw_jpeg("cell/cell-.jpg",0x105,2);
    resource_draw_jpeg("cell/cell--.jpg",0x135,2);
  }
  return;
}

