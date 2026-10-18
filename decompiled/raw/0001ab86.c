/* Original address: 0x0001ab86; Ghidra pseudocode, not compilable reconstructed source. */

int FUN_ram_0001ab86(int param_1,char *param_2,undefined4 *param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  char *pcVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  char cVar9;
  uint uVar10;
  int iVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  uint uVar14;
  undefined4 uVar15;
  int iVar16;
  char *pcVar17;
  char acStack_34 [8];
  
  gp = &__global_pointer;
  iVar16 = 0;
  iVar2 = 0;
  while (*param_2 != '\0') {
    if (*param_2 == '%') {
      cVar1 = param_2[1];
      pcVar17 = param_2 + 1;
      if (cVar1 == '%') {
        FUN_ram_0001a74a(param_1,0x25);
        iVar2 = iVar2 + 1;
      }
      else {
        if (cVar1 == '\0') break;
        if (cVar1 == '+') {
          pcVar17 = param_2 + 2;
LAB_ram_0001ac26:
          uVar14 = 0;
        }
        else if (cVar1 < ',') {
          if (cVar1 != '#') goto LAB_ram_0001ac26;
          pcVar17 = param_2 + 2;
          uVar14 = 4;
        }
        else if (cVar1 == '-') {
          pcVar17 = param_2 + 2;
          uVar14 = 1;
        }
        else {
          if (cVar1 != '0') goto LAB_ram_0001ac26;
          pcVar17 = param_2 + 2;
          uVar14 = 2;
        }
        cVar1 = *pcVar17;
        if (cVar1 == '+') {
LAB_ram_0001ac92:
          pcVar17 = pcVar17 + 1;
        }
        else if (cVar1 < ',') {
          if (cVar1 == '#') {
            uVar14 = uVar14 | 4;
            goto LAB_ram_0001ac92;
          }
        }
        else if (cVar1 == '-') {
          pcVar17 = pcVar17 + 1;
          uVar14 = 1;
        }
        else if (cVar1 == '0') {
          uVar14 = uVar14 | 2;
          goto LAB_ram_0001ac92;
        }
        iVar3 = 0;
        while( true ) {
          uVar10 = (int)*pcVar17 - 0x30;
          if (9 < (uVar10 & 0xff)) break;
          pcVar17 = pcVar17 + 1;
          iVar3 = iVar3 * 10 + uVar10;
        }
        iVar11 = 6;
        if (*pcVar17 == 0x2e) {
          iVar11 = 0;
          while( true ) {
            pcVar17 = pcVar17 + 1;
            iVar16 = iVar11;
            if (9 < ((int)*pcVar17 - 0x30U & 0xff)) break;
            iVar11 = iVar11 * 10 + ((int)*pcVar17 - 0x30U);
          }
        }
        cVar1 = *pcVar17;
        if (cVar1 == 'j') {
LAB_ram_0001ad8a:
          pcVar17 = pcVar17 + 1;
LAB_ram_0001acf0:
          iVar6 = 0;
        }
        else {
          if (cVar1 < 'k') {
            if (cVar1 == 'L') goto LAB_ram_0001ad8a;
            if (cVar1 == 'h') {
              iVar6 = 0;
              if (pcVar17[1] == 'h') {
                pcVar17 = pcVar17 + 2;
                iVar6 = 0;
              }
              goto LAB_ram_0001ad28;
            }
            goto LAB_ram_0001acf0;
          }
          if ((cVar1 == 't') || (cVar1 == 'z')) goto LAB_ram_0001ad8a;
          iVar6 = 0;
          if (cVar1 == 'l') {
            iVar6 = 3;
            if (pcVar17[1] == 'l') {
              pcVar17 = pcVar17 + 1;
              iVar6 = 4;
            }
            pcVar17 = pcVar17 + 1;
          }
        }
LAB_ram_0001ad28:
        cVar1 = *pcVar17;
        uVar15 = 0x61;
        if (cVar1 < 'a') {
          uVar15 = 0x41;
        }
        if (cVar1 < 'h') {
          if ('d' < cVar1) {
LAB_ram_0001aeba:
            puVar8 = (undefined4 *)((int)param_3 + 7U & 0xfffffff8);
            param_3 = puVar8 + 2;
            iVar3 = FUN_ram_0001aa86(param_1,*puVar8,puVar8[1],10,(iVar3 - iVar11) + -1,iVar11,
                                     uVar14,uVar15);
LAB_ram_0001ae38:
            iVar2 = iVar2 + iVar3;
            goto LAB_ram_0001abf4;
          }
          if (cVar1 < 'H') {
            if ('D' < cVar1) goto LAB_ram_0001aeba;
            if (cVar1 != 'C') goto LAB_ram_0001abf4;
          }
          else if (cVar1 != 'c') {
            if (cVar1 < 'd') {
              cVar9 = 'X';
              goto LAB_ram_0001ad9e;
            }
            if (iVar6 != 4) {
              uVar4 = *param_3;
              uVar13 = 1;
              goto LAB_ram_0001ae46;
            }
            param_3 = (undefined4 *)((int)param_3 + 7U & 0xfffffff8);
            uVar4 = *param_3;
            uVar7 = param_3[1];
            uVar13 = 1;
LAB_ram_0001ae30:
            param_3 = param_3 + 2;
            uVar12 = 10;
LAB_ram_0001ae32:
            iVar3 = FUN_ram_0001a966(param_1,uVar4,uVar7,uVar12,uVar13,iVar3,uVar14,uVar15,0);
            goto LAB_ram_0001ae38;
          }
          acStack_34[1] = 0;
          acStack_34[0] = (char)*param_3;
          pcVar5 = acStack_34;
          iVar11 = 0;
        }
        else {
          if (cVar1 != 's') {
            if (cVar1 < 't') {
              if (cVar1 != 'o') {
                uVar14 = uVar14 | 4;
                if (cVar1 == 'p') goto LAB_ram_0001adc4;
                goto LAB_ram_0001abf4;
              }
              if (iVar6 == 4) {
                param_3 = (undefined4 *)((int)param_3 + 7U & 0xfffffff8);
                uVar4 = *param_3;
                uVar7 = param_3[1];
                param_3 = param_3 + 2;
                uVar13 = 0;
                uVar12 = 8;
                goto LAB_ram_0001ae32;
              }
              uVar4 = *param_3;
              uVar13 = 0;
              uVar7 = 8;
            }
            else if (cVar1 == 'u') {
              if (iVar6 == 4) {
                param_3 = (undefined4 *)((int)param_3 + 7U & 0xfffffff8);
                uVar4 = *param_3;
                uVar7 = param_3[1];
                uVar13 = 0;
                goto LAB_ram_0001ae30;
              }
              uVar4 = *param_3;
              uVar13 = 0;
LAB_ram_0001ae46:
              uVar7 = 10;
            }
            else {
              cVar9 = 'x';
LAB_ram_0001ad9e:
              if (cVar1 != cVar9) goto LAB_ram_0001abf4;
LAB_ram_0001adc4:
              if (iVar6 == 4) {
                param_3 = (undefined4 *)((int)param_3 + 7U & 0xfffffff8);
                uVar4 = *param_3;
                uVar7 = param_3[1];
                param_3 = param_3 + 2;
                uVar13 = 0;
                uVar12 = 0x10;
                goto LAB_ram_0001ae32;
              }
              uVar4 = *param_3;
              uVar13 = 0;
              uVar7 = 0x10;
            }
            param_3 = param_3 + 1;
            iVar3 = FUN_ram_0001a874(param_1,uVar4,uVar7,uVar13,iVar3,uVar14,uVar15,iVar16);
            goto LAB_ram_0001ae38;
          }
          pcVar5 = "(null)";
          iVar11 = iVar16;
          if ((char *)*param_3 != (char *)0x0) {
            pcVar5 = (char *)*param_3;
          }
        }
        param_3 = param_3 + 1;
        iVar3 = FUN_ram_0001a794(param_1,pcVar5,iVar3,uVar14,iVar11);
        iVar2 = iVar2 + iVar3;
      }
    }
    else {
      FUN_ram_0001a74a(param_1);
      iVar2 = iVar2 + 1;
      pcVar17 = param_2;
    }
LAB_ram_0001abf4:
    param_2 = pcVar17 + 1;
  }
  if (param_1 != 0) {
    FUN_ram_0001a74a(param_1,0);
  }
  return iVar2;
}

