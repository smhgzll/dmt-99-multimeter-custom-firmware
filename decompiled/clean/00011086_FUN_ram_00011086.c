/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00011086; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00011086; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00011086(int param_1)

{
  bool bVar1;
  char cVar2;
  uint unaff_s0;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  char cVar7;
  uint uVar8;
  int iVar9;
  uint unaff_s2;
  uint unaff_s3;
  undefined4 uVar10;
  int iVar11;
  uint uVar12;
  uint uStack_4c;
  
  gp = &__global_pointer;
  if (button_event == '\x05') {
    button_event = '\0';
    uVar4 = param_1 - 6U & 0xff;
    switch(uVar4) {
    case 0:
      uVar10 = 0x1e;
      if (display_layout == '\0') {
        uVar10 = 0x2b;
      }
      uVar12 = DAT_ram_20000032 & 0x7fff;
      if ((short)DAT_ram_20000032 < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_20000034 & 0x7fff;
      if ((short)DAT_ram_20000034 < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_0001111a:
      do {
        if (button_event == '\x06') goto LAB_ram_00011132;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_00011132;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -999) {
          uVar12 = 0xfffffc19;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uStack_4c <= (int)uVar12) {
          uVar12 = uStack_4c - 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_0001111a;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x101,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_0001111a;
            }
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf2,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    case 1:
      uVar10 = 0x39;
      if (display_layout == '\0') {
        uVar10 = 0x40;
      }
      uVar12 = DAT_ram_20000034 & 0x7fff;
      if ((short)DAT_ram_20000034 < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_20000032 & 0x7fff;
      if ((short)DAT_ram_20000032 < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_0001149e:
      do {
        if (button_event == '\x06') goto LAB_ram_000114b6;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_000114b6;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -999) {
          uVar12 = 0xfffffc19;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uVar12 <= (int)uStack_4c) {
          uVar12 = uStack_4c + 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_0001149e;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x101,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_0001149e;
            }
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf2,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    case 2:
      uVar10 = 0x6a;
      if (display_layout == '\0') {
        uVar10 = 0x6c;
      }
      uVar12 = DAT_ram_20000036 & 0x7fff;
      if ((short)DAT_ram_20000036 < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_20000038 & 0x7fff;
      if ((short)DAT_ram_20000038 < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_00011828:
      do {
        if (button_event == '\x06') goto LAB_ram_00011840;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_00011840;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -999) {
          uVar12 = 0xfffffc19;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uStack_4c <= (int)uVar12) {
          uVar12 = uStack_4c - 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_00011828;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x107,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_00011828;
            }
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf8,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    case 3:
      uVar10 = 0x86;
      if (display_layout == '\0') {
        uVar10 = 0x82;
      }
      uVar12 = DAT_ram_20000038 & 0x7fff;
      if ((short)DAT_ram_20000038 < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_20000036 & 0x7fff;
      if ((short)DAT_ram_20000036 < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_00011c1c:
      do {
        if (button_event == '\x06') goto LAB_ram_00011c34;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_00011c34;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -999) {
          uVar12 = 0xfffffc19;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uVar12 <= (int)uStack_4c) {
          uVar12 = uStack_4c + 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_00011c1c;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x107,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf8,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_00011c1c;
            }
            FUN_ram_00008266(0x107,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf8,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    case 4:
      uVar10 = 0xb8;
      if (display_layout == '\0') {
        uVar10 = 0xac;
      }
      uVar12 = DAT_ram_2000003a & 0x7fff;
      if ((short)DAT_ram_2000003a < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_2000003c & 0x7fff;
      if ((short)DAT_ram_2000003c < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_00012016:
      do {
        if (button_event == '\x06') goto LAB_ram_0001202e;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_0001202e;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -0x37) {
          uVar12 = 0xffffffc9;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uStack_4c <= (int)uVar12) {
          uVar12 = uStack_4c - 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_00012016;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x101,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_00012016;
            }
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf2,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    case 5:
      uVar10 = 0xd5;
      if (display_layout == '\0') {
        uVar10 = 0xc3;
      }
      uVar12 = DAT_ram_2000003c & 0x7fff;
      if ((short)DAT_ram_2000003c < 0) {
        uVar12 = -uVar12;
      }
      uStack_4c = DAT_ram_2000003a & 0x7fff;
      if ((short)DAT_ram_2000003a < 0) {
        uStack_4c = -uStack_4c;
      }
      cVar2 = '\x01';
      uVar4 = 0;
      cVar7 = '\0';
LAB_ram_0001239c:
      do {
        if (button_event == '\x06') goto LAB_ram_000123b4;
        if (button_event == '\x05') {
          button_event = '\0';
          goto LAB_ram_000123b4;
        }
        if (button_event == '\x03') {
          cVar2 = cVar2 + '\x01';
          button_event = '\0';
          if (cVar2 == '\x05') {
            cVar2 = '\x01';
          }
        }
        else if (button_event == '\x04') {
          cVar2 = cVar2 + -1;
          button_event = '\0';
          if (cVar2 == '\0') {
            cVar2 = '\x04';
          }
        }
        if (button_event == '\x01') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 - 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 - 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 - 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if (button_event == '\x02') {
          button_event = '\0';
          if (cVar2 == '\x03') {
            uVar12 = uVar12 + 10;
          }
          else if (cVar2 == '\x04') {
            uVar12 = uVar12 + 1;
          }
          else if (cVar2 == '\x02') {
            uVar12 = uVar12 + 100;
          }
          else {
            uVar12 = -uVar12;
          }
        }
        if ((int)uVar12 < -0x37) {
          uVar12 = 0xffffffc9;
        }
        if (999 < (int)uVar12) {
          uVar12 = 999;
        }
        if ((int)uVar12 <= (int)uStack_4c) {
          uVar12 = uStack_4c + 1;
        }
        uVar8 = uVar12 & 0xffff;
        uVar3 = uVar8;
        if ((int)uVar12 < 0) {
          uVar3 = uVar8 * -0x10000 >> 0x10;
        }
        unaff_s2 = (uVar3 / 100) % 10;
        unaff_s0 = uVar3 % 10;
        unaff_s3 = (uVar3 / 10) % 10;
        if ((uVar4 != uVar12) || (bVar1 = cVar7 != cVar2, uVar4 = uVar12, cVar7 = cVar2, bVar1)) {
          iVar9 = unaff_s3 + 0x30;
          iVar5 = unaff_s0 + 0x30;
          iVar11 = unaff_s2 + 0x30;
          uVar4 = uVar8;
          if (cVar2 == '\x01') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
            uVar6 = 0x2b;
            if ((int)uVar12 < 1) {
              uVar6 = 0x2d;
            }
            FUN_ram_000083d6(0xd4,uVar10,uVar6,0x10,0xffff,0x1f,0);
            cVar7 = cVar2;
            goto LAB_ram_0001239c;
          }
          if (cVar2 == '\x02') {
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
            FUN_ram_000083d6(0xe3,uVar10,iVar11,0x10,0xffff,0x1f,0);
          }
          else {
            if (cVar2 != '\x03') {
              FUN_ram_000083d6(0x101,uVar10,iVar5,0x10,0xffff,0x1f,0);
              FUN_ram_00008266(0xf2,uVar10,iVar9,0x10,0xfd40,0);
              FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
              uVar6 = 0x2b;
              if ((int)uVar12 < 1) {
                uVar6 = 0x2d;
              }
              FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
              cVar2 = '\x04';
              cVar7 = cVar2;
              goto LAB_ram_0001239c;
            }
            FUN_ram_00008266(0x101,uVar10,iVar5,0x10,0xfd40,0);
            FUN_ram_000083d6(0xf2,uVar10,unaff_s3 + 0x30,0x10,0xffff,0x1f,0);
            FUN_ram_00008266(0xe3,uVar10,iVar11,0x10,0xfd40,0);
          }
          uVar6 = 0x2d;
          if (0 < (int)uVar12) {
            uVar6 = 0x2b;
          }
          FUN_ram_00008266(0xd4,uVar10,uVar6,0x10,0xfd40,0);
          cVar7 = cVar2;
        }
      } while( true );
    }
  }
  else {
    uVar4 = 0;
  }
switchD_ram_000110ca_default:
  wch_restore_12(uVar4);
  return;
LAB_ram_000123b4:
  FUN_ram_00008266(0x101,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_2000003c = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_2000003c = (ushort)uVar12 & 0x7fff;
  }
  goto switchD_ram_000110ca_default;
LAB_ram_0001202e:
  FUN_ram_00008266(0x101,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_2000003a = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_2000003a = (ushort)uVar12;
  }
  goto switchD_ram_000110ca_default;
LAB_ram_00011c34:
  FUN_ram_00008266(0x107,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf8,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_20000038 = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_20000038 = (ushort)uVar12 & 0x7fff;
  }
  goto switchD_ram_000110ca_default;
LAB_ram_00011840:
  FUN_ram_00008266(0x107,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf8,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xef,uVar10,0x2e,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_20000036 = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_20000036 = (ushort)uVar12;
  }
  goto switchD_ram_000110ca_default;
LAB_ram_000114b6:
  FUN_ram_00008266(0x101,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_20000034 = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_20000034 = (ushort)uVar12 & 0x7fff;
  }
  goto switchD_ram_000110ca_default;
LAB_ram_00011132:
  FUN_ram_00008266(0x101,uVar10,unaff_s0 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar10,unaff_s3 + 0x30 & 0xff,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar10,unaff_s2 + 0x30 & 0xff,0x10,0xffff,0);
  if ((int)uVar12 < 1) {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2d,0x10,0xffff,0);
    DAT_ram_20000032 = -(ushort)uVar12 | 0x8000;
  }
  else {
    uVar4 = FUN_ram_00008266(0xd4,uVar10,0x2b,0x10,0xffff,0);
    DAT_ram_20000032 = (ushort)uVar12;
  }
  goto switchD_ram_000110ca_default;
}

