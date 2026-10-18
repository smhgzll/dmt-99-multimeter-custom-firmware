/* Original address: 0x00016242; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00016242(void)

{
  uint uVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  char *pcVar6;
  undefined4 uVar7;
  
  gp = &__global_pointer;
  uVar3 = settings_auto_off_access(0,0);
  FUN_ram_00014408(6);
  if (display_layout == '\0') {
    display_fill_rectangle(0x96,0xf,300,0x14,0);
    display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
    display_fill_rectangle(0x8c,0x14,0x136,0xda,0);
    FUN_ram_000081ce(0x90,0x3d,0x12a,200,8,0x4228);
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00008e46(0xb8,0x27,&DAT_ram_00024f60);
    }
    else {
      FUN_ram_00008588(0xa0,0x27,"AUTO shut_down",0xffff,0xffff,0x10,1);
    }
    sVar2 = 0x4a;
    uVar1 = 0;
    do {
      if (uVar3 == uVar1) {
        display_fill_rectangle(0xa0,sVar2,0xb4,sVar2 + 0x14,0x1c9f);
        uVar7 = 0x1c9f;
      }
      else {
        FUN_ram_00008194(0xa0,sVar2,0xb4,sVar2 + 0x14,0xffff);
        uVar7 = 0xffff;
      }
      if (uVar1 == 0) {
        pcVar6 = "OFF";
        uVar5 = 0x4c;
      }
      else if (uVar1 == 1) {
        pcVar6 = "5min";
        uVar5 = 0x6b;
      }
      else if (uVar1 == 2) {
        pcVar6 = "15min";
        uVar5 = 0x8a;
      }
      else {
        pcVar6 = "20min";
        uVar5 = 0xa9;
      }
      FUN_ram_00008588(0xc9,uVar5,pcVar6,uVar7,uVar7,0x10,1);
      sVar2 = sVar2 + 0x1f;
      uVar1 = uVar1 + 1 & 0xff;
    } while (uVar1 != 4);
  }
  else {
    display_fill_rectangle(0x83,0,0x140,0xf0,0);
    iVar4 = settings_language_access(0,0);
    if (iVar4 == 0) {
      FUN_ram_00008e46(0x8b,9,&DAT_ram_00024f60,0xffff,0xffff,0x18);
      FUN_ram_000083d6(0xe7,9,0x3a,0x18,0xffff,0xffff,1);
    }
    else {
      FUN_ram_00008588(0x8b,0xf,"AUTO shut_down:",0xffff,0xffff,0x10,1);
    }
    sVar2 = 0x29;
    uVar1 = 0;
    do {
      if (uVar3 == uVar1) {
        display_fill_rectangle(0x99,sVar2,0xac,sVar2 + 0x13,0xfd40);
        uVar7 = 0xfd40;
      }
      else {
        FUN_ram_00008194(0x99,sVar2,0xac,sVar2 + 0x13,0xffff);
        uVar7 = 0xffff;
      }
      if (uVar1 == 0) {
        pcVar6 = "OFF";
        uVar5 = 0x26;
      }
      else if (uVar1 == 1) {
        pcVar6 = "5min";
        uVar5 = 0x52;
      }
      else if (uVar1 == 2) {
        pcVar6 = "15min";
        uVar5 = 0x7e;
      }
      else {
        pcVar6 = "20min";
        uVar5 = 0xaa;
      }
      FUN_ram_00008588(0xb4,uVar5,pcVar6,uVar7,uVar7,0x18,1);
      sVar2 = sVar2 + 0x2c;
      uVar1 = uVar1 + 1 & 0xff;
    } while (uVar1 != 4);
  }
  return;
}

