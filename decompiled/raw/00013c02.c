/* Original address: 0x00013c02; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00013c02(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  char *pcVar6;
  undefined *puVar7;
  undefined4 uVar8;
  
  gp = &__global_pointer;
  iVar1 = settings_language_access(0,0);
  settings_field_3_4_access(0,0);
  uVar2 = settings_layout_access(0,0);
  uVar3 = settings_auto_off_access(0,0);
  if (param_1 == 2) {
    if (param_2 == 2) {
      FUN_ram_00008194(0xa0,0xb1,0xb4,0xc5,0xffff);
      display_fill_rectangle(0xa1,0xb2,0xb3,0xc4,0);
      FUN_ram_00008e46(0xcf,0xb0,&DAT_ram_00025234,0xffff,0xffff,0x18,1);
      settings_temperature_access(1,0);
      display_fill_rectangle(0xa0,0x8c,0xb4,0xa0,0x1c9f);
      puVar7 = &DAT_ram_00025230;
      uVar5 = 0x8a;
    }
    else {
      FUN_ram_00008194(0xa0,0x8c,0xb4,0xa0,0xffff);
      display_fill_rectangle(0xa1,0x8d,0xb3,0x9f,0);
      FUN_ram_00008e46(0xcf,0x8a,&DAT_ram_00025230,0xffff,0xffff,0x18,1);
      if (param_2 != 3) {
        gp = &__global_pointer;
        return;
      }
      settings_temperature_access(1,1);
      display_fill_rectangle(0xa0,0xb1,0xb4,0xc5,0x1c9f);
      puVar7 = &DAT_ram_00025234;
      uVar5 = 0xb0;
    }
    uVar8 = 0x18;
    uVar4 = 0xcf;
LAB_ram_00013e44:
    FUN_ram_00008e46(uVar4,uVar5,puVar7,0x1c9f,0x1c9f,uVar8,1);
  }
  else {
    if (param_1 == 3) {
      if (uVar3 == 0) {
        FUN_ram_00008194(0xa0,0x4a,0xb4,0x5e,0xffff);
        display_fill_rectangle(0xa1,0x4b,0xb3,0x5d,0);
        pcVar6 = "OFF";
        uVar5 = 0x4c;
      }
      else if (uVar3 == 1) {
        FUN_ram_00008194(0xa0,0x69,0xb4,0x7d,0xffff);
        display_fill_rectangle(0xa1,0x6a,0xb3,0x7c,0);
        pcVar6 = "5min";
        uVar5 = 0x6b;
      }
      else if (uVar3 == 2) {
        FUN_ram_00008194(0xa0,0x88,0xb4,0x9c,0xffff);
        display_fill_rectangle(0xa1,0x89,0xb3,0x9b,0);
        pcVar6 = "15min";
        uVar5 = 0x8a;
      }
      else {
        FUN_ram_00008194(0xa0,0xa7,0xb4,0xbb,0xffff);
        display_fill_rectangle(0xa1,0xa8,0xb3,0xba,0);
        pcVar6 = "20min";
        uVar5 = 0xa9;
      }
      FUN_ram_00008588(0xc9,uVar5,pcVar6,0xffff,0xffff,0x10,1);
      if (param_2 == 1) {
        settings_auto_off_access(1,0);
        display_fill_rectangle(0xa0,0x4a,0xb4,0x5e,0x1c9f);
        pcVar6 = "OFF";
        uVar5 = 0x4c;
      }
      else if (param_2 == 2) {
        settings_auto_off_access(1,1);
        display_fill_rectangle(0xa0,0x69,0xb4,0x7d,0x1c9f);
        pcVar6 = "5min";
        uVar5 = 0x6b;
      }
      else if (param_2 == 3) {
        settings_auto_off_access(1,2);
        display_fill_rectangle(0xa0,0x88,0xb4,0x9c,0x1c9f);
        pcVar6 = "15min";
        uVar5 = 0x8a;
      }
      else {
        settings_auto_off_access(1,3);
        display_fill_rectangle(0xa0,0xa7,0xb4,0xbb,0x1c9f);
        pcVar6 = "20min";
        uVar5 = 0xa9;
      }
      uVar4 = 0xc9;
    }
    else {
      if (param_1 != 1) {
        gp = &__global_pointer;
        return;
      }
      if ((param_2 - 1U & 0xff) < 2) {
        if ((uVar2 & 0xff) == 0) {
          FUN_ram_00008194(0xa5,0x34,0xba,0x3d,0xffff);
          display_fill_rectangle(0xa6,0x35,0xb9,0x3c,0);
          if (iVar1 == 0) {
            FUN_ram_00008e46(0x9f,0x47,&DAT_ram_00025078,0xffff,0xffff,0x10,1);
            pcVar6 = "1";
            uVar5 = 0x47;
            uVar4 = 0xbf;
          }
          else {
            pcVar6 = "Theme1";
            uVar5 = 0x45;
            uVar4 = 0xa1;
          }
        }
        else {
          FUN_ram_00008194(0xfc,0x34,0x111,0x3d,0xffff);
          display_fill_rectangle(0xfd,0x35,0x110,0x3c,0);
          if (iVar1 == 0) {
            FUN_ram_00008e46(0xf6,0x47,&DAT_ram_00025078,0xffff,0xffff,0x10,1);
            pcVar6 = "2";
            uVar5 = 0x47;
            uVar4 = 0x116;
          }
          else {
            pcVar6 = "Theme2";
            uVar5 = 0x45;
            uVar4 = 0xee;
          }
        }
        FUN_ram_00008588(uVar4,uVar5,pcVar6,0xffff,0xffff,0x10,1);
        if (param_2 == 1) {
          settings_layout_access(1,0);
          display_fill_rectangle(0xa5,0x34,0xba,0x3d,0x1c9f);
          if (iVar1 == 0) {
            FUN_ram_00008e46(0x9f,0x47,&DAT_ram_00025078,0x1c9f,0x1c9f,0x10,1);
            pcVar6 = "1";
            uVar5 = 0x47;
            uVar4 = 0xbf;
          }
          else {
            pcVar6 = "Theme1";
            uVar5 = 0x45;
            uVar4 = 0xa1;
          }
        }
        else {
          settings_layout_access(1,1);
          display_fill_rectangle(0xfc,0x34,0x111,0x3d,0x1c9f);
          if (iVar1 == 0) {
            FUN_ram_00008e46(0xf6,0x47,&DAT_ram_00025078,0x1c9f,0x1c9f,0x10,1);
            pcVar6 = "2";
            uVar5 = 0x47;
            uVar4 = 0x116;
          }
          else {
            pcVar6 = "Theme2";
            uVar5 = 0x45;
            uVar4 = 0xee;
          }
        }
      }
      else {
        if (iVar1 == 0) {
          FUN_ram_00008194(0xa0,0x8b,0xb4,0x9f,0xffff);
          display_fill_rectangle(0xa1,0x8c,0xb3,0x9e,0);
          FUN_ram_00008e46(0xc6,0x8d,&DAT_ram_00025098,0xffff,0xffff,0x10,1);
        }
        else {
          FUN_ram_00008194(0xa0,0xb3,0xb4,199,0xffff);
          display_fill_rectangle(0xa1,0xb4,0xb3,0xc6,0);
          FUN_ram_00008588(0xc6,0xb5,"English",0xffff,0xffff,0x10,1);
        }
        if (param_2 == 3) {
          settings_language_access(1,0);
          display_fill_rectangle(0xa0,0x8b,0xb4,0x9f,0x1c9f);
          uVar8 = 0x10;
          puVar7 = &DAT_ram_00025098;
          uVar5 = 0x8d;
          uVar4 = 0xc6;
          goto LAB_ram_00013e44;
        }
        if (param_2 != 4) {
          gp = &__global_pointer;
          return;
        }
        settings_language_access(1,1);
        display_fill_rectangle(0xa0,0xb3,0xb4,199,0x1c9f);
        pcVar6 = "English";
        uVar5 = 0xb5;
        uVar4 = 0xc6;
      }
    }
    FUN_ram_00008588(uVar4,uVar5,pcVar6,0x1c9f,0x1c9f,0x10,1);
  }
  return;
}

