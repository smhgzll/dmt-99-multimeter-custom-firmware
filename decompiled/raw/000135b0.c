/* Original address: 0x000135b0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_000135b0(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined *puVar6;
  char *pcVar7;
  
  gp = &__global_pointer;
  iVar1 = settings_language_access(0,0);
  settings_field_3_4_access(0,0);
  uVar2 = settings_layout_access(0,0);
  uVar3 = settings_auto_off_access(0,0);
  if (param_1 == 2) {
    if (param_2 != 2) {
      FUN_ram_00008194(0x99,0xa1,0xac,0xb4,0xffff);
      display_fill_rectangle(0x9a,0xa2,0xab,0xb3,0);
      FUN_ram_00008e46(0xb4,0x9e,&DAT_ram_00025230,0xffff,0xffff,0x18,1);
      if (param_2 != 3) {
        gp = &__global_pointer;
        return;
      }
      settings_temperature_access(1,1);
      display_fill_rectangle(0x99,0xcd,0xac,0xe0,0xfd40);
      iVar1 = 0x234;
      uVar5 = 0xca;
      goto LAB_ram_0001388a;
    }
    FUN_ram_00008194(0x99,0xcd,0xac,0xe0,0xffff);
    display_fill_rectangle(0x9a,0xce,0xab,0xdf,0);
    FUN_ram_00008e46(0xb4,0xca,&DAT_ram_00025234,0xffff,0xffff,0x18,1);
    settings_temperature_access(1,0);
    display_fill_rectangle(0x99,0xa1,0xac,0xb4,0xfd40);
    iVar1 = 0x230;
LAB_ram_00013886:
    uVar5 = 0x9e;
LAB_ram_0001388a:
    FUN_ram_00008e46(0xb4,uVar5,"menu/russian_.jpg" + iVar1 + 0xc,0xfd40,0xfd40,0x18,1);
    gp = &__global_pointer;
    return;
  }
  if (param_1 == 3) {
    if (uVar3 == 0) {
      FUN_ram_00008194(0x99,0x29,0xac,0x3c,0xffff);
      display_fill_rectangle(0x9a,0x2a,0xab,0x3b,0);
      pcVar7 = "OFF";
      uVar5 = 0x26;
    }
    else if (uVar3 == 1) {
      FUN_ram_00008194(0x99,0x55,0xac,0x68,0xffff);
      display_fill_rectangle(0x9a,0x56,0xab,0x67,0);
      pcVar7 = "5min";
      uVar5 = 0x52;
    }
    else if (uVar3 == 2) {
      FUN_ram_00008194(0x99,0x81,0xac,0x94,0xffff);
      display_fill_rectangle(0x9a,0x82,0xab,0x93,0);
      pcVar7 = "15min";
      uVar5 = 0x7e;
    }
    else {
      FUN_ram_00008194(0x99,0xad,0xac,0xc0,0xffff);
      display_fill_rectangle(0x9a,0xae,0xab,0xbf,0);
      pcVar7 = "20min";
      uVar5 = 0xaa;
    }
    FUN_ram_00008588(0xb4,uVar5,pcVar7,0xffff,0xffff,0x18,1);
    if (param_2 == 1) {
      settings_auto_off_access(1,0);
      FUN_ram_00008588(0xb4,0x26,&DAT_ram_000250a8,0xfd40,0xfd40,0x18,1);
      uVar5 = 0x3c;
      uVar4 = 0x29;
    }
    else if (param_2 == 2) {
      settings_auto_off_access(1,1);
      FUN_ram_00008588(0xb4,0x52,&DAT_ram_000250ac,0xfd40,0xfd40,0x18,1);
      uVar5 = 0x68;
      uVar4 = 0x55;
    }
    else if (param_2 == 3) {
      settings_auto_off_access(1,2);
      FUN_ram_00008588(0xb4,0x7e,"15min",0xfd40,0xfd40,0x18,1);
      uVar5 = 0x94;
      uVar4 = 0x81;
    }
    else {
      settings_auto_off_access(1,3);
      FUN_ram_00008588(0xb4,0xaa,"20min",0xfd40,0xfd40,0x18,1);
      uVar5 = 0xc0;
      uVar4 = 0xad;
    }
    display_fill_rectangle(0x99,uVar4,0xac,uVar5,0xfd40);
    gp = &__global_pointer;
    return;
  }
  if (param_1 != 1) {
    gp = &__global_pointer;
    return;
  }
  if ((param_2 - 1U & 0xff) < 2) {
    if ((uVar2 & 0xff) == 0) {
      FUN_ram_00008194(0x99,0x29,0xac,0x3c,0xffff);
      display_fill_rectangle(0x9a,0x2a,0xab,0x3b,0);
      if (iVar1 != 0) {
        pcVar7 = "Theme1";
        uVar5 = 0x26;
        goto LAB_ram_000136ca;
      }
      puVar6 = &DAT_ram_00025078;
      uVar5 = 0x26;
LAB_ram_0001364a:
      FUN_ram_00008e46(0xb4,uVar5,puVar6);
    }
    else {
      FUN_ram_00008194(0x99,0x55,0xac,0x68,0xffff);
      display_fill_rectangle(0x9a,0x56,0xab,0x67,0);
      if (iVar1 == 0) {
        puVar6 = &DAT_ram_00025088;
        uVar5 = 0x52;
        goto LAB_ram_0001364a;
      }
      pcVar7 = "Theme2";
      uVar5 = 0x52;
LAB_ram_000136ca:
      FUN_ram_00008588(0xb4,uVar5,pcVar7,0xffff,0xffff,0x18,1);
    }
    if (param_2 == 1) {
      settings_layout_access(1,0);
      display_fill_rectangle(0x99,0x29,0xac,0x3c,0xfd40);
      if (iVar1 != 0) {
        pcVar7 = "Theme1";
        uVar5 = 0x26;
        goto LAB_ram_00013a9a;
      }
      FUN_ram_00008e46(0xb4,0x26,&DAT_ram_00025078,0xfd40,0xfd40,0x18,1);
      uVar5 = 0x26;
    }
    else {
      settings_layout_access(1,1);
      display_fill_rectangle(0x99,0x55,0xac,0x68,0xfd40);
      if (iVar1 != 0) {
        pcVar7 = "Theme2";
        uVar5 = 0x52;
        goto LAB_ram_00013a9a;
      }
      FUN_ram_00008e46(0xb4,0x52,&DAT_ram_00025078,0xfd40,0xfd40,0x18,1);
      uVar5 = 0x52;
    }
    pcVar7 = "1";
    uVar4 = 0xe4;
  }
  else {
    if (iVar1 == 0) {
      FUN_ram_00008194(0x99,0xa1,0xac,0xb4,0xffff);
      display_fill_rectangle(0x9a,0xa2,0xab,0xb3,0);
      FUN_ram_00008e46(0xb4,0x9e,&DAT_ram_00025098,0xffff,0xffff,0x18,1);
    }
    else {
      FUN_ram_00008194(0x99,0xcd,0xac,0xe0,0xffff);
      display_fill_rectangle(0x9a,0xce,0xab,0xdf,0);
      FUN_ram_00008588(0xb4,0xca,"English",0xffff,0xffff,0x18,1);
    }
    if (param_2 == 3) {
      settings_language_access(1,0);
      display_fill_rectangle(0x99,0xa1,0xac,0xb4,0xfd40);
      iVar1 = 0x98;
      goto LAB_ram_00013886;
    }
    if (param_2 != 4) {
      gp = &__global_pointer;
      return;
    }
    settings_language_access(1,1);
    display_fill_rectangle(0x99,0xcd,0xac,0xe0,0xfd40);
    pcVar7 = "English";
    uVar5 = 0xca;
LAB_ram_00013a9a:
    uVar4 = 0xb4;
  }
  FUN_ram_00008588(uVar4,uVar5,pcVar7,0xfd40,0xfd40,0x18,1);
  return;
}

