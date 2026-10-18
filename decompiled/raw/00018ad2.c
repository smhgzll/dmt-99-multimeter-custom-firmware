/* Original address: 0x00018ad2; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00018ad2(int param_1,char param_2,int param_3)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined *puVar7;
  char *pcVar8;
  undefined4 uVar9;
  undefined8 uVar10;
  
  gp = &__global_pointer;
  if (param_1 != 0) {
    iVar3 = 0;
    do {
      uVar10 = FUN_ram_0001d072(iVar3);
      uVar10 = FUN_ram_0001c3b8((int)uVar10,(int)((ulonglong)uVar10 >> 0x20),0,0x403e8000);
      FUN_ram_0001b61e((int)uVar10,(int)((ulonglong)uVar10 >> 0x20),0,0x404e0000);
      uVar4 = FUN_ram_0001d016();
      iVar3 = iVar3 + 1;
      FUN_ram_00008050(0,uVar4,0x77,uVar4,0x4228);
    } while (iVar3 != 5);
    FUN_ram_00008050(0x78,0x1d,0x78,0xd2,0x4228);
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008b5c(6,0x24,&DAT_ram_000253d0,0xffff);
      FUN_ram_00008b5c(0x24,0x24,&DAT_ram_000253d4,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x43,0x24,&DAT_ram_000253d8,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x61,0x24,&DAT_ram_000253dc,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(6,0x42,&DAT_ram_000253e0,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x24,0x42,&DAT_ram_00025464,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x43,0x42,&DAT_ram_000253e4,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x61,0x42,&DAT_ram_000253e8,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(5,0x61,&DAT_ram_000253ec,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x23,0x61,&DAT_ram_000253f0,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x42,0x61,&DAT_ram_000253f4,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x60,0x61,&DAT_ram_000253f8,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(5,0x7f,&DAT_ram_000253fc,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x23,0x7f,&DAT_ram_00025400,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x42,0x7f,&DAT_ram_000253e4,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x60,0x7f,&DAT_ram_000253e8,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(5,0x9e,&DAT_ram_00025404,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x17,0x9e,&DAT_ram_00025408,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x29,0x9e,&DAT_ram_0002540c,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x3b,0x9e,&DAT_ram_00025410,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x4e,0x9e,&DAT_ram_000253e4,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x61,0x9e,&DAT_ram_000253e8,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(5,0xbc,&DAT_ram_00025414,0xffff,0xffff,0x10,1);
      FUN_ram_00008b5c(0x60,0xbc,&DAT_ram_00025418,0xffff,0xffff,0x10,1);
    }
    else {
      FUN_ram_00008588(6,0x24,"Mode selection",0xffff,0,0x10,1);
      FUN_ram_00008588(0x18,0x3d,"Threshold",0xffff,0,0x10,1);
      FUN_ram_00008588(0x1c,0x49,"settings",0xffff,0,0x10,1);
      FUN_ram_00008588(6,0x61,"History record",0xffff,0,0x10,1);
      FUN_ram_00008588(0,0x7f,"System settings",0xffff,0,0x10,1);
      FUN_ram_00008588(0x28,0x9e,"Reset",0xffff,0,0x10,1);
      FUN_ram_00008588(0x28,0xbc,"About",0xffff,0,0x10,1);
    }
  }
  switch(param_2 + -1) {
  case '\0':
    uVar9 = 0x3b;
    uVar6 = 0x1d;
    break;
  case '\x01':
    uVar9 = 0x59;
    uVar6 = 0x3d;
    break;
  case '\x02':
    uVar9 = 0x78;
    uVar6 = 0x5b;
    break;
  case '\x03':
    uVar9 = 0x96;
    uVar6 = 0x7a;
    break;
  case '\x04':
    uVar9 = 0xb5;
    uVar6 = 0x98;
    break;
  case '\x05':
    uVar9 = 0xd2;
    uVar6 = 0xb7;
    break;
  default:
    goto switchD_ram_00018e2c_default;
  }
  display_fill_rectangle(0,uVar6,0x77,uVar9,0x1c9f);
switchD_ram_00018e2c_default:
  bVar1 = false;
  uVar9 = 0;
  switch(param_2 + -1) {
  case '\0':
    break;
  case '\x01':
    goto switchD_ram_000192de_caseD_1;
  case '\x02':
    goto switchD_ram_000192de_caseD_2;
  case '\x03':
    goto switchD_ram_000192de_caseD_3;
  case '\x04':
    goto switchD_ram_000192de_caseD_4;
  case '\x05':
    goto switchD_ram_000192de_caseD_5;
  default:
    goto switchD_ram_00018f8c_default;
  }
switchD_ram_000192de_caseD_0:
  iVar3 = settings_language_access(0,0);
  if (iVar3 != 0) {
    pcVar8 = "Mode selection";
    uVar6 = 0x24;
    goto LAB_ram_000192ee;
  }
  FUN_ram_00008b5c(6,0x24,&DAT_ram_000253d0,uVar9);
  FUN_ram_00008b5c(0x24,0x24,&DAT_ram_000253d4,uVar9,uVar9,0x10,1);
  FUN_ram_00008b5c(0x43,0x24,&DAT_ram_000253d8,uVar9,uVar9,0x10,1);
  puVar7 = &DAT_ram_000253dc;
  uVar6 = 0x24;
LAB_ram_00019014:
  uVar5 = 0x61;
LAB_ram_00019018:
  FUN_ram_00008b5c(uVar5,uVar6,puVar7,uVar9,uVar9,0x10,1);
switchD_ram_00018f8c_default:
  if (bVar1) {
    return;
  }
  bVar1 = true;
  bVar2 = true;
  if (param_3 == 0) {
    return;
  }
  switch(param_3 - 1U & 0xff) {
  case 0:
    display_fill_rectangle(0,0x1d,0x77,0x3b,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
    goto switchD_ram_000192de_caseD_0;
  case 1:
    display_fill_rectangle(0,0x3d,0x77,0x59,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
switchD_ram_000192de_caseD_1:
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008b5c(6,0x42,&DAT_ram_000253e0);
      FUN_ram_00008b5c(0x24,0x42,&DAT_ram_00025464,uVar9,uVar9,0x10,1);
      FUN_ram_00008b5c(0x43,0x42,&DAT_ram_000253e4,uVar9,uVar9,0x10,1);
      puVar7 = &DAT_ram_000253e8;
      uVar6 = 0x42;
      goto LAB_ram_00019014;
    }
    FUN_ram_00008588(0x18,0x3d,"Threshold",uVar9,uVar9,0x10,1);
    pcVar8 = "settings";
    uVar6 = 0x49;
    uVar5 = 0x1c;
    break;
  case 2:
    display_fill_rectangle(0,0x5b,0x77,0x78,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
switchD_ram_000192de_caseD_2:
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008b5c(5,0x61,&DAT_ram_000253ec,uVar9,uVar9,0x10,1);
      FUN_ram_00008b5c(0x23,0x61,&DAT_ram_000253f0,uVar9,uVar9,0x10,1);
      FUN_ram_00008b5c(0x42,0x61,&DAT_ram_000253f4,uVar9,uVar9,0x10,1);
      puVar7 = &DAT_ram_000253f8;
      uVar6 = 0x61;
      goto LAB_ram_00019130;
    }
    pcVar8 = "History record";
    uVar6 = 0x61;
LAB_ram_000192ee:
    uVar5 = 6;
    break;
  case 3:
    display_fill_rectangle(0,0x7a,0x77,0x96,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
switchD_ram_000192de_caseD_3:
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) goto code_r0x0001915e;
    pcVar8 = "System settings";
    uVar6 = 0x7f;
    uVar5 = 0;
    break;
  case 4:
    display_fill_rectangle(0,0x98,0x77,0xb5,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
switchD_ram_000192de_caseD_4:
    iVar3 = settings_language_access(0,0);
    if (iVar3 != 0) {
      pcVar8 = "Reset";
      uVar6 = 0x9e;
      goto LAB_ram_00019350;
    }
    FUN_ram_00008b5c(5,0x9e,&DAT_ram_00025404,uVar9,uVar9,0x10,1);
    FUN_ram_00008b5c(0x17,0x9e,&DAT_ram_00025408,uVar9,uVar9,0x10,1);
    FUN_ram_00008b5c(0x29,0x9e,&DAT_ram_0002540c,uVar9,uVar9,0x10,1);
    FUN_ram_00008b5c(0x3b,0x9e,&DAT_ram_00025410,uVar9,uVar9,0x10,1);
    FUN_ram_00008b5c(0x4e,0x9e,&DAT_ram_000253e4,uVar9,uVar9,0x10,1);
    puVar7 = &DAT_ram_000253e8;
    uVar6 = 0x9e;
    goto LAB_ram_00019014;
  case 5:
    display_fill_rectangle(0,0xb7,0x77,0xd2,0);
    uVar9 = 0xffff;
    bVar1 = bVar2;
switchD_ram_000192de_caseD_5:
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008b5c(5,0xbc,&DAT_ram_00025414,uVar9,uVar9,0x10,1);
      puVar7 = &DAT_ram_00025418;
      uVar6 = 0xbc;
      goto LAB_ram_00019130;
    }
    pcVar8 = "About";
    uVar6 = 0xbc;
LAB_ram_00019350:
    uVar5 = 0x28;
    break;
  default:
    goto switchD_ram_00018f8c_default;
  }
  FUN_ram_00008588(uVar5,uVar6,pcVar8,uVar9,uVar9,0x10,1);
  goto switchD_ram_00018f8c_default;
code_r0x0001915e:
  FUN_ram_00008b5c(5,0x7f,&DAT_ram_000253fc,uVar9,uVar9,0x10,1);
  FUN_ram_00008b5c(0x23,0x7f,&DAT_ram_00025400,uVar9,uVar9,0x10,1);
  FUN_ram_00008b5c(0x42,0x7f,&DAT_ram_000253e4,uVar9,uVar9,0x10,1);
  puVar7 = &DAT_ram_000253e8;
  uVar6 = 0x7f;
LAB_ram_00019130:
  uVar5 = 0x60;
  goto LAB_ram_00019018;
}

