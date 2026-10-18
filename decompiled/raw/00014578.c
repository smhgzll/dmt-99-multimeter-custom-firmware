/* Original address: 0x00014578; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00014578(int param_1,int param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  undefined8 uVar11;
  undefined1 auStack_48 [20];
  
  gp = &__global_pointer;
  iVar8 = 7;
  uVar9 = param_1 * 5 & 0xff;
  uVar2 = uVar9 - 4;
  if (display_layout != '\0') {
    iVar8 = 0;
  }
  uVar1 = 0x35;
  do {
    uVar2 = uVar2 & 0xff;
    if ((uVar9 + param_2 + -4 & 0xff) == uVar2) {
      wch_restore_12();
      return;
    }
    history_decode_entry(uVar2);
    if (uVar2 < 10) {
      FUN_ram_0000969e(auStack_48,"%c%d%c%c",0x30,uVar2,0x2e,0);
    }
    else {
      FUN_ram_0000969e(auStack_48,"%d%c%c",uVar2,0x2e,0);
    }
    uVar2 = uVar2 + 1;
    FUN_ram_000085cc(0x84,uVar1,0x1e,0x14,0x10,0xffff,auStack_48);
    if (param_3 == 0) {
      FUN_ram_00014408(param_1);
    }
    switch(DAT_ram_20000228 + -10) {
    case '\0':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa9;
        pcVar4 = "history/cap_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/cap.jpg";
      }
      break;
    case '\x01':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa9;
        pcVar4 = "history/res_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/res.jpg";
      }
      break;
    case '\x02':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa2;
        pcVar4 = "history/_v_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/_v.jpg";
      }
      break;
    case '\x03':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa2;
        pcVar4 = "history/~v_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/~v.jpg";
      }
      break;
    case '\x04':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/ol_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/ol.jpg";
      }
      break;
    case '\x05':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa9;
        pcVar4 = "history/hz_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/hz.jpg";
      }
      break;
    case '\x06':
    case '\a':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa2;
        pcVar4 = "history/_a_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/_a.jpg";
      }
      break;
    case '\b':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1;
        uVar7 = 0xa9;
        pcVar4 = "history/temp_c.jpg";
      }
      else {
        uVar7 = 0xa0;
        pcVar4 = "history/temp.jpg";
        uVar6 = uVar1;
      }
      resource_draw_jpeg(pcVar4,uVar7,uVar6 & 0xffff);
      uVar6 = settings_temperature_access(0,0);
      if (uVar6 == 1) {
        uVar11 = FUN_ram_0001e2e2(DAT_ram_2000022c);
        uVar11 = FUN_ram_0001c3b8((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0xcccccccd,0x3ffccccc
                                 );
        FUN_ram_0001b61e((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0,0x40400000);
        DAT_ram_2000022c = FUN_ram_0001e38e();
      }
    default:
      goto switchD_ram_0001465c_caseD_9;
    case '\v':
    case '\f':
      iVar3 = settings_language_access(0,0);
      if (iVar3 == 0) {
        uVar6 = uVar1 + 1 & 0xffff;
        uVar7 = 0xa2;
        pcVar4 = "history/~a_c.jpg";
      }
      else {
        uVar6 = uVar1 & 0xffff;
        uVar7 = 0x9b;
        pcVar4 = "history/~a.jpg";
      }
    }
    resource_draw_jpeg(pcVar4,uVar7,uVar6);
switchD_ram_0001465c_caseD_9:
    uVar6 = DAT_ram_2000022c;
    iVar3 = FUN_ram_0001dabc(DAT_ram_2000022c,0);
    uVar10 = uVar6;
    if (iVar3 < 0) {
      uVar10 = uVar6 ^ 0x80000000;
    }
    iVar3 = FUN_ram_0001d9da(uVar6,0x476eee00);
    if (iVar3 == 0) {
      uVar7 = 0x2e;
      uVar5 = 0x30;
LAB_ram_00014766:
      FUN_ram_0000969e(auStack_48,&DAT_ram_00023788,uVar5,uVar7,0x4c,0);
    }
    else {
      iVar3 = FUN_ram_0001d9da(uVar6,0x477fff00);
      if (iVar3 == 0) {
        uVar7 = 0x30;
        uVar5 = 0x2e;
        goto LAB_ram_00014766;
      }
      uVar11 = FUN_ram_0001e2e2(uVar6);
      iVar3 = FUN_ram_0001da36(uVar10,0x447a0000);
      if (iVar3 < 0) {
        iVar3 = FUN_ram_0001da36(uVar10,0x42c80000);
        if (iVar3 < 0) {
          iVar3 = FUN_ram_0001da36(uVar10,0x41200000);
          if (iVar3 < 0) {
            pcVar4 = "%0.3f";
          }
          else {
            pcVar4 = "%0.2f";
          }
        }
        else {
          pcVar4 = "%0.1f";
        }
      }
      else {
        pcVar4 = "%0.0f";
      }
      FUN_ram_0000969e(auStack_48,pcVar4,(int)uVar11,(int)((ulonglong)uVar11 >> 0x20));
    }
    FUN_ram_000085cc(0xf2,uVar1,0x50,0x14,0x10,0xffff,auStack_48);
    switch(DAT_ram_20000229 + -6) {
    case '\0':
      uVar7 = 0x56;
      goto LAB_ram_000149a2;
    case '\x01':
      uVar7 = 0x56;
      goto LAB_ram_000149b4;
    case '\x02':
      uVar7 = 0x41;
      goto LAB_ram_000149de;
    case '\x03':
      uVar7 = 0x41;
      goto LAB_ram_000149b4;
    case '\x04':
      uVar7 = 0x41;
      goto LAB_ram_000149a2;
    case '\x05':
      uVar7 = 0x58;
      goto LAB_ram_000149a2;
    case '\x06':
      uVar7 = 0x58;
      uVar5 = 0x6b;
      break;
    case '\a':
      uVar7 = 0x58;
      uVar5 = 0x4d;
      break;
    default:
      goto switchD_ram_000147ac_caseD_8;
    case '\t':
      uVar7 = 0x46;
      uVar5 = 0x6e;
      break;
    case '\n':
      uVar7 = 0x46;
LAB_ram_000149de:
      uVar5 = 0x59;
      break;
    case '\v':
      uVar7 = 0x46;
LAB_ram_000149b4:
      uVar5 = 0x6d;
      break;
    case '\f':
      uVar7 = 0x7a;
      uVar5 = 0x48;
      break;
    case '\r':
      uVar7 = 0x6b;
      goto LAB_ram_00014a2e;
    case '\x0e':
      uVar7 = 0x4d;
LAB_ram_00014a2e:
      FUN_ram_0000969e(auStack_48,&DAT_ram_00023788,uVar7,0x48,0x7a,0);
      goto LAB_ram_00014a3c;
    case '\x0f':
      uVar6 = settings_temperature_access(0,0);
      uVar7 = 0x5a;
      if (uVar6 != 0) {
        uVar7 = 0x4a;
      }
LAB_ram_000149a2:
      FUN_ram_0000969e(auStack_48,&DAT_ram_0002378c,uVar7,0);
      goto LAB_ram_00014a3c;
    }
    FUN_ram_0000969e(auStack_48,"%c%c%c",uVar5,uVar7,0);
LAB_ram_00014a3c:
    FUN_ram_000085cc(0x122,uVar1,0x14,0x3c,0x10,0xffff,auStack_48);
switchD_ram_000147ac_caseD_8:
    uVar1 = uVar1 + (0x2a - iVar8);
  } while( true );
}

