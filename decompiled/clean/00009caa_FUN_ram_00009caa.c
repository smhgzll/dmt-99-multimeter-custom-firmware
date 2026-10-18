/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00009caa; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00009caa; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00009caa(void)

{
  uchar uVar1;
  uint uVar2;
  undefined1 uVar3;
  uint uVar4;
  undefined *puVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  char cVar9;
  int iVar10;
  char cVar11;
  char cVar12;
  undefined8 uVar13;
  uchar auStack_48 [4];
  char acStack_44 [8];
  undefined1 auStack_3c [4];
  char cStack_38;
  
  gp = &__global_pointer;
  measurement_decode_segments(auStack_48);
  iVar10 = 0;
  do {
    uVar1 = auStack_48[iVar10];
    if ((char)uVar1 < '\n') {
      acStack_44[iVar10] = uVar1 + '0';
    }
    else if (uVar1 == '\n') {
      acStack_44[iVar10] = 'A';
    }
    else if (uVar1 == '\v') {
      acStack_44[iVar10] = 'U';
    }
    else if (uVar1 == '\f') {
      acStack_44[iVar10] = 'T';
    }
    else if (uVar1 == '\r') {
      acStack_44[iVar10] = 'O';
    }
    else if (uVar1 == '\x0e') {
      acStack_44[iVar10] = 'L';
    }
    else if (uVar1 == '\x0f') {
      acStack_44[iVar10] = 'E';
    }
    else if (uVar1 == '\x10') {
      acStack_44[iVar10] = '-';
    }
    else if (uVar1 == '\x11') {
      acStack_44[iVar10] = ' ';
      if (((measurement_mode == '\x12') || (measurement_mode == '\x11')) ||
         (measurement_mode == '\x16')) {
        acStack_44[iVar10] = '0';
      }
    }
    else {
      acStack_44[iVar10] = 'X';
    }
    iVar10 = iVar10 + 1;
  } while (iVar10 != 4);
  uVar6 = 0x2d;
  if ((measurement_dma_frame.status_or_segments0 & 0x10) == 0) {
    uVar6 = 0x20;
  }
  cVar11 = acStack_44[2];
  cVar12 = acStack_44[3];
  if ((measurement_dma_frame.packed_segments[0] & 0x10) == 0) {
    cVar9 = acStack_44[1];
    if ((measurement_dma_frame.packed_segments[1] & 0x10) == 0) {
      cVar11 = acStack_44[3];
      cVar12 = 0x20;
      if ((measurement_dma_frame.packed_segments[2] & 0x10) != 0) {
        cVar11 = 0x2e;
        cVar12 = acStack_44[3];
      }
    }
    else {
      acStack_44[2] = 0x2e;
    }
  }
  else {
    cVar9 = 0x2e;
    acStack_44[2] = acStack_44[1];
  }
  FUN_ram_0000969e(&measurement_text,"%c%c%c%c%c%c%c",uVar6,acStack_44[0],cVar9,acStack_44[2],cVar11
                   ,cVar12,0);
  uVar2 = measurement_value_bits;
  if (application_view == '\x01') {
    if (DAT_ram_2000027c != '\0') {
      if (DAT_ram_200001f4 == '\x01') {
        uVar2 = FUN_ram_0001dddc(measurement_value_bits,DAT_ram_200001f8);
      }
      if ((measurement_mode == '\x12') && (uVar4 = settings_temperature_access(0,0), uVar4 == 1)) {
        uVar13 = FUN_ram_0001e2e2(uVar2);
        uVar13 = FUN_ram_0001c3b8((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0xcccccccd,0x3ffccccc
                                 );
        FUN_ram_0001b61e((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0,0x40400000);
        uVar2 = FUN_ram_0001e38e();
      }
      FUN_ram_000099cc(uVar2,&measurement_text);
    }
    if (display_layout == '\0') {
      FUN_ram_00010bd0();
    }
    else {
      FUN_ram_000109e4(&measurement_text);
    }
    iVar10 = FUN_ram_0001dabc(uVar2,0);
    if (iVar10 < 0) {
      uVar2 = uVar2 ^ 0x80000000;
    }
    iVar10 = FUN_ram_0001da36(uVar2,0x447a0000);
    uVar6 = 0x42c80000;
    if (iVar10 < 1) {
      iVar10 = FUN_ram_0001da36(uVar2,0x42c80000);
      if (iVar10 < 1) {
        iVar10 = FUN_ram_0001dabc(uVar2,0x41200000);
        if (iVar10 < 0) {
          uVar2 = FUN_ram_0001db48(uVar2,0x41200000);
        }
        goto LAB_ram_00009dc6;
      }
      uVar6 = 0x41200000;
    }
    uVar2 = FUN_ram_0001d728(uVar2,uVar6);
LAB_ram_00009dc6:
    if (display_layout == '\0') {
      FUN_ram_0001a468(&DAT_ram_000263ec,0,0x17,0x1c9f,0x4228,uVar2);
      FUN_ram_0001a468(&DAT_ram_000269a4,0x109,0x17,0x1c9f,0x4228,uVar2);
    }
    else {
      uVar3 = FUN_ram_0001e1c6(uVar2);
      FUN_ram_0001066c(uVar3);
    }
    return;
  }
  if (application_view != '\x02') {
    if (application_view != '\x03') {
      return;
    }
    if (DAT_ram_2000027c == '\x01') {
      if ((measurement_mode == '\x12') && (uVar4 = settings_temperature_access(0,0), uVar4 == 1)) {
        uVar13 = FUN_ram_0001e2e2(uVar2);
        uVar13 = FUN_ram_0001c3b8((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0xcccccccd,0x3ffccccc
                                 );
        FUN_ram_0001b61e((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0,0x40400000);
        uVar2 = FUN_ram_0001e38e();
      }
      FUN_ram_000099cc(uVar2,&measurement_text);
    }
    if (display_layout == '\0') {
      FUN_ram_00010bd0();
    }
    else {
      FUN_ram_000109e4(&measurement_text);
    }
    FUN_ram_00013264();
    return;
  }
  iVar10 = 0x28;
  if (display_layout != '\0') {
    iVar10 = 0;
  }
  if (DAT_ram_2000027c == '\x01') {
    if ((measurement_mode == '\x12') && (uVar4 = settings_temperature_access(0,0), uVar4 == 1)) {
      uVar13 = FUN_ram_0001e2e2(uVar2);
      uVar13 = FUN_ram_0001c3b8((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0xcccccccd,0x3ffccccc);
      FUN_ram_0001b61e((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0,0x40400000);
      uVar2 = FUN_ram_0001e38e();
    }
    FUN_ram_000099cc(uVar2,&measurement_text);
  }
  FUN_ram_000085cc(0x46 - iVar10,0xf,200,100,0x20,0xfd40,&measurement_text);
  if ((measurement_mode == '\x12') ||
     (((measurement_mode == '\x11' || (measurement_mode == '\x16')) && (measurement_unit_code == 8))
     )) {
    display_fill_rectangle(0x96U - iVar10 & 0xffff,0xf,0x9eU - iVar10 & 0xffff,0x2f,0);
  }
  if (measurement_mode == '\v') {
    iVar10 = FUN_ram_0001d9da(uVar2,0);
    if ((iVar10 != 0) && (cStack_38 != 'L')) {
      uVar4 = (uint)measurement_unit_code;
      if ((int)uVar4 < (int)DAT_ram_200001c0) {
        if (uVar4 == 0xb) {
          uVar6 = 0x20;
          uVar7 = 0x58;
        }
        else if (uVar4 == 0xc) {
          uVar6 = 0x58;
          uVar7 = 0x6b;
        }
        else {
          uVar6 = 0x58;
          uVar7 = 0x4d;
        }
        DAT_ram_200001c0 = uVar4;
        FUN_ram_0000969e(auStack_3c,"%c%c%c",uVar7,uVar6,0);
        FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
      uVar4 = (uint)measurement_unit_code;
      if ((int)DAT_ram_200001bc < (int)uVar4) {
        if (uVar4 == 0xb) {
          uVar6 = 0x20;
          uVar7 = 0x58;
LAB_ram_0000a0f0:
          puVar5 = auStack_3c;
        }
        else {
          if (uVar4 == 0xc) {
            uVar6 = 0x58;
            uVar7 = 0x6b;
            goto LAB_ram_0000a0f0;
          }
          uVar6 = 0x58;
          uVar7 = 0x4d;
          puVar5 = &measurement_text;
        }
        DAT_ram_200001bc = uVar4;
        FUN_ram_0000969e(puVar5,"%c%c%c",uVar7,uVar6,0);
        FUN_ram_000085cc(0xd1,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
    }
    iVar10 = FUN_ram_0001da36(DAT_ram_200001b4,uVar2);
    if ((0 < iVar10) && (measurement_unit_code == DAT_ram_200001c0)) {
      DAT_ram_200001b4 = uVar2;
      FUN_ram_00013504(measurement_value_bits,&measurement_text);
      display_fill_rectangle(0x3f,0xdb,0x6b,0xe9,0);
      FUN_ram_000085cc(0x3f,0xdb,0x50,0x28,0x10,0xffff,&measurement_text);
    }
    iVar10 = FUN_ram_0001dabc(DAT_ram_200001b0,uVar2);
    if ((-1 < iVar10) || (uVar4 = measurement_value_bits, measurement_unit_code != DAT_ram_200001bc)
       ) goto LAB_ram_0000a1de;
  }
  else {
    if (measurement_mode == '\n') {
      uVar4 = (uint)measurement_unit_code;
      if ((int)uVar4 < (int)DAT_ram_200001c0) {
        if (uVar4 == 0x10) {
          uVar6 = 0x59;
        }
        else if (uVar4 == 0xf) {
          uVar6 = 0x6e;
        }
        else {
          uVar6 = 0x6d;
        }
        DAT_ram_200001c0 = uVar4;
        FUN_ram_0000969e(auStack_3c,"%c%c%c",uVar6,0x46,0);
        FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
      uVar4 = (uint)measurement_unit_code;
      if ((int)DAT_ram_200001bc < (int)uVar4) {
        DAT_ram_200001bc = uVar4;
        if (uVar4 == 0x10) {
          uVar6 = 0x46;
          uVar7 = 0x59;
        }
        else if (uVar4 == 0xf) {
          uVar6 = 0x46;
          uVar7 = 0x6e;
        }
        else {
          uVar6 = 0x46;
LAB_ram_0000a462:
          uVar7 = 0x6d;
        }
LAB_ram_0000a37c:
        FUN_ram_0000969e(auStack_3c,"%c%c%c",uVar7,uVar6,0);
        FUN_ram_000085cc(0xd1,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
LAB_ram_0000a3a2:
      iVar10 = FUN_ram_0001da36(DAT_ram_200001b4,uVar2);
      if ((0 < iVar10) && (measurement_unit_code == DAT_ram_200001c0)) {
LAB_ram_0000a3c4:
        DAT_ram_200001b4 = uVar2;
        FUN_ram_00013504(measurement_value_bits,&measurement_text);
        display_fill_rectangle(0x3f,0xdb,0x6b,0xe9,0);
        FUN_ram_000085cc(0x3f,0xdb,0x50,0x28,0x10,0xffff,&measurement_text);
      }
    }
    else {
      if ((measurement_mode == '\x11') || (measurement_mode == '\x16')) {
        uVar4 = (uint)measurement_unit_code;
        if ((int)uVar4 < (int)DAT_ram_200001c0) {
          if (uVar4 == 8) {
            uVar6 = 0x59;
          }
          else {
            uVar6 = 0x6d;
          }
          DAT_ram_200001c0 = uVar4;
          FUN_ram_0000969e(auStack_3c,"%c%c%c",uVar6,0x41,0);
          FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
        }
        uVar4 = (uint)measurement_unit_code;
        if ((int)DAT_ram_200001bc < (int)uVar4) {
          DAT_ram_200001bc = uVar4;
          if (uVar4 != 8) {
            uVar6 = 0x41;
            goto LAB_ram_0000a462;
          }
          uVar6 = 0x41;
          uVar7 = 0x75;
          goto LAB_ram_0000a37c;
        }
        goto LAB_ram_0000a3a2;
      }
      if (measurement_mode != '\x0f') {
        if (measurement_mode == '\x0e') goto LAB_ram_0000a1de;
        iVar10 = FUN_ram_0001da36(DAT_ram_200001b4,uVar2);
        if (0 < iVar10) {
          DAT_ram_200001b4 = uVar2;
          FUN_ram_00013504(uVar2,&measurement_text);
          display_fill_rectangle(0x3f,0xdb,0x6b,0xe9,0);
          FUN_ram_000085cc(0x3f,0xdb,0x50,0x28,0x10,0xffff,&measurement_text);
        }
        iVar10 = FUN_ram_0001dabc(DAT_ram_200001b0,uVar2);
        uVar4 = uVar2;
        if (-1 < iVar10) goto LAB_ram_0000a1de;
        goto LAB_ram_0000a1a6;
      }
      uVar4 = (uint)measurement_unit_code;
      if ((int)uVar4 < (int)DAT_ram_200001c0) {
        if (uVar4 == 0x12) {
          uVar6 = 0x20;
          uVar7 = 0x7a;
          uVar8 = 0x48;
        }
        else if (uVar4 == 0x13) {
          uVar6 = 0x7a;
          uVar7 = 0x48;
          uVar8 = 0x4b;
        }
        else {
          uVar6 = 0x7a;
          uVar7 = 0x48;
          uVar8 = 0x4d;
        }
        DAT_ram_200001c0 = uVar4;
        FUN_ram_0000969e(auStack_3c,&DAT_ram_00023788,uVar8,uVar7,uVar6,0);
        FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
      uVar4 = (uint)measurement_unit_code;
      if ((int)DAT_ram_200001bc < (int)uVar4) {
        if (uVar4 == 0x12) {
          uVar6 = 0x20;
          uVar7 = 0x7a;
          uVar8 = 0x48;
        }
        else if (uVar4 == 0x13) {
          uVar6 = 0x7a;
          uVar7 = 0x48;
          uVar8 = 0x4b;
        }
        else {
          uVar6 = 0x7a;
          uVar7 = 0x48;
          uVar8 = 0x4d;
        }
        DAT_ram_200001bc = uVar4;
        FUN_ram_0000969e(auStack_3c,&DAT_ram_00023788,uVar8,uVar7,uVar6,0);
        FUN_ram_000085cc(0xd1,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
      }
      iVar10 = FUN_ram_0001da36(DAT_ram_200001b4,uVar2);
      if ((0 < iVar10) && (measurement_unit_code == DAT_ram_200001c0)) goto LAB_ram_0000a3c4;
    }
    iVar10 = FUN_ram_0001dabc(DAT_ram_200001b0,uVar2);
    if ((-1 < iVar10) || (uVar4 = measurement_value_bits, measurement_unit_code != DAT_ram_200001bc)
       ) goto LAB_ram_0000a1de;
  }
LAB_ram_0000a1a6:
  DAT_ram_200001b0 = uVar2;
  FUN_ram_00013504(uVar4,&measurement_text);
  display_fill_rectangle(0xa5,0xdb,0xd1,0xe9,0);
  FUN_ram_000085cc(0xa5,0xdb,0x50,0x28,0x10,0xffff,&measurement_text);
LAB_ram_0000a1de:
  if (DAT_ram_200001b8 != measurement_mode) {
    DAT_ram_200001b8 = measurement_mode;
    DAT_ram_200001b4 = 0x461c3c00;
    DAT_ram_200001b0 = 0xc61c3c00;
    DAT_ram_200001bc = 0xffffd8f1;
    DAT_ram_200001c0 = 9999;
    FUN_ram_00017cce(0,1);
    display_fill_rectangle(0x3f,0xdb,0x84,0xea,0);
    display_fill_rectangle(0xa5,0xdb,0xea,0xea,0);
    if (measurement_mode != '\r') {
      return;
    }
    FUN_ram_0000969e(auStack_3c,&DAT_ram_00023788,0x56,0x20,0x20,0);
    FUN_ram_000085cc(0xd1,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
    FUN_ram_000085cc(0x6b,0xdb,0x14,0x3c,0x10,0xffff,auStack_3c);
    return;
  }
  if (DAT_ram_2000023e != '\0') {
    return;
  }
  if (DAT_ram_20000290 == 'L') {
    return;
  }
  if (measurement_mode == '\x0e') {
    return;
  }
  uVar13 = FUN_ram_0001e2e2(uVar2);
  FUN_ram_00017cce((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),0);
  return;
}

