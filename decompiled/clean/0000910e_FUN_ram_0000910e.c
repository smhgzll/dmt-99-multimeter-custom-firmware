/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000910e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000910e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */
/* WARNING: Removing unreachable block (ram,0x000094c2) */
/* WARNING: Removing unreachable block (ram,0x000094d0) */
/* WARNING: Removing unreachable block (ram,0x0000941e) */
/* WARNING: Removing unreachable block (ram,0x0000942c) */
/* WARNING: Restarted to delay deadcode elimination for space: ram */

void FUN_ram_0000910e(byte *param_1,int param_2,byte *param_3,uint *param_4)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  bool bVar4;
  byte *pbVar5;
  uint uVar6;
  byte *pbVar7;
  byte *pbVar8;
  uint uVar9;
  char cVar10;
  byte bVar11;
  uint uVar12;
  uint uVar13;
  undefined8 *puVar14;
  uint uVar15;
  byte *pbVar16;
  uint uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  uint uVar20;
  int iVar21;
  undefined8 uVar22;
  undefined8 uVar23;
  uint uStack_6c;
  byte *pbStack_68;
  
  gp = &__global_pointer;
  pbVar16 = param_1 + param_2;
  uVar23 = 0;
  uVar20 = 0;
  pbStack_68 = (byte *)0x0;
  bVar3 = 0;
  DAT_ram_200001f0 = param_1;
LAB_ram_00009148:
  pbVar7 = param_1;
  bVar11 = *param_3;
  pbVar5 = param_3 + 1;
  if (bVar11 == 0) {
    *pbVar7 = 0;
    pbVar8 = pbVar7 + 1;
    goto LAB_ram_0000915a;
  }
  if (bVar11 == 0x25) {
    uVar17 = (uint)(char)param_3[1];
    bVar11 = 0x25;
    pbVar5 = param_3 + 2;
    if (uVar17 == 0x25) goto LAB_ram_0000916e;
    if (uVar17 == 0x30) {
      uVar17 = (uint)(char)param_3[2];
      pbVar5 = param_3 + 3;
      bVar4 = false;
      bVar2 = true;
    }
    else {
      bVar4 = false;
      bVar2 = false;
      if (uVar17 == 0x2d) {
        pbVar5 = param_3 + 3;
        uVar17 = (uint)(char)param_3[2];
        bVar4 = true;
        bVar2 = false;
      }
    }
    uStack_6c = 0;
    while (((uVar17 & 0xff) - 0x30 & 0xff) < 10) {
      uStack_6c = (uStack_6c * 10 + (uVar17 & 0xff)) - 0x30 & 0xff;
      uVar17 = (uint)(char)*pbVar5;
      pbVar5 = pbVar5 + 1;
    }
    uVar15 = 6;
    if (uVar17 == 0x2e) {
      bVar11 = *pbVar5;
      uVar15 = 0;
      while( true ) {
        pbVar5 = pbVar5 + 1;
        uVar17 = (uint)(char)bVar11;
        uVar13 = (uint)bVar11;
        if (9 < (uVar13 - 0x30 & 0xff)) break;
        bVar11 = *pbVar5;
        uVar15 = (uVar15 * 10 + uVar13) - 0x30 & 0xff;
      }
    }
    if (uVar17 == 0x6c) {
      uVar17 = (uint)(char)*pbVar5;
      pbVar5 = pbVar5 + 1;
    }
    pbVar8 = pbVar7;
    if (uVar17 == 0) goto LAB_ram_0000915a;
    if (uVar17 == 0x66) {
      puVar14 = (undefined8 *)((uint)((int)param_4 + 7) & 0xfffffff8);
      uVar18 = *(undefined4 *)puVar14;
      uVar19 = *(undefined4 *)((int)puVar14 + 4);
      uVar23 = *puVar14;
      param_4 = (uint *)(puVar14 + 1);
      iVar21 = FUN_ram_0001c302(uVar18,uVar19,0,0);
      if (iVar21 < 0) {
        uVar23 = FUN_ram_0001c870(0,0,uVar18,uVar19);
      }
      uVar13 = (uint)(iVar21 < 0);
      if (uVar15 != 0) {
        uVar13 = uVar13 + 1;
      }
      uVar20 = FUN_ram_0001d016((int)uVar23,(int)((ulonglong)uVar23 >> 0x20));
      iVar21 = FUN_ram_000090e6();
      uVar6 = iVar21 + uVar15 & 0xff;
LAB_ram_000092a8:
      uVar18 = (undefined4)((ulonglong)uVar23 >> 0x20);
      param_3 = pbVar5;
      uVar9 = uVar6;
      if (bVar4) {
        if (uVar17 == 0x66) goto LAB_ram_000094ba;
        if ((int)uVar17 < 0x67) {
          pbVar5 = pbVar7;
          if (uVar17 == 99) goto LAB_ram_000093c6;
          if (uVar17 == 100) goto LAB_ram_00009440;
          if (uVar17 == 0x58) goto LAB_ram_00009650;
        }
        else {
          if (uVar17 == 0x75) goto LAB_ram_00009440;
          if (uVar17 == 0x78) goto LAB_ram_00009650;
          if (uVar17 == 0x73) goto LAB_ram_0000940a;
        }
      }
      else {
        pbVar5 = pbVar7;
        if (uVar6 + uVar13 < uStack_6c) {
          bVar11 = 0x20;
          if (bVar2) {
            bVar11 = 0x30;
          }
          while ((int)((int)pbVar5 - (int)pbVar7 & 0xffffU) < (int)(uStack_6c - (uVar6 + uVar13))) {
            pbVar8 = pbVar5 + 1;
            *pbVar5 = bVar11;
            pbVar5 = pbVar8;
            if ((pbVar16 <= pbVar8) && (param_2 != 0)) goto LAB_ram_0000915a;
          }
        }
        pbVar7 = pbVar5;
        if (uVar17 == 0x66) {
LAB_ram_000094ba:
          uVar20 = FUN_ram_0001d016((int)uVar23,uVar18);
          for (uVar17 = ~uVar15 + uVar6; (uVar17 & 0xffff) != 0xffff; uVar17 = (uVar17 & 0xffff) - 1
              ) {
            pbVar8 = pbVar7 + 1;
            uVar12 = uVar17 & 0xff;
            uVar9 = 1;
            while (uVar12 = uVar12 - 1, uVar12 != 0xffffffff) {
              uVar9 = uVar9 * 10;
            }
            *pbVar7 = (char)((uVar20 / uVar9) % 10) + 0x30;
            if ((pbVar16 <= pbVar8) && (param_2 != 0)) goto LAB_ram_0000915a;
            pbVar7 = pbVar8;
          }
          if (uVar15 != 0) {
            *pbVar7 = 0x2e;
            uVar22 = FUN_ram_0001d0e4(uVar20);
            uVar23 = FUN_ram_0001c870((int)uVar23,uVar18,(int)uVar22,
                                      (int)((ulonglong)uVar22 >> 0x20));
            iVar21 = 1;
            uVar20 = uVar15;
            while (uVar20 = uVar20 - 1, uVar20 != 0xffffffff) {
              iVar21 = iVar21 * 10;
            }
            uVar22 = FUN_ram_0001d072(iVar21);
            uVar23 = FUN_ram_0001c3b8((int)uVar22,(int)((ulonglong)uVar22 >> 0x20),(int)uVar23,
                                      (int)((ulonglong)uVar23 >> 0x20));
            uVar23 = FUN_ram_0001b61e((int)uVar23,(int)((ulonglong)uVar23 >> 0x20),0,0x3fe00000);
            uVar20 = FUN_ram_0001d016();
            pbVar5 = pbVar7 + 1;
            do {
              pbVar7 = pbVar5 + 1;
              uVar17 = uVar15 - 1 & 0xff;
              uVar9 = 1;
              while (uVar17 = uVar17 - 1, uVar17 != 0xffffffff) {
                uVar9 = uVar9 * 10;
              }
              *pbVar5 = (char)((uVar20 / uVar9) % 10) + 0x30;
              if ((pbVar16 <= pbVar7) && (pbVar8 = pbVar7, param_2 != 0)) goto LAB_ram_0000915a;
              uVar15 = uVar15 - 1 & 0xffff;
              pbVar5 = pbVar7;
            } while (uVar15 != 0);
          }
        }
        else {
          param_1 = pbVar7;
          if ((int)uVar17 < 0x67) {
            pbVar5 = pbVar7;
            if (uVar17 != 99) {
              if (uVar17 == 100) goto LAB_ram_00009440;
              if (uVar17 == 0x58) goto LAB_ram_00009650;
              goto LAB_ram_00009148;
            }
LAB_ram_000093c6:
            pbVar7 = pbVar5 + 1;
            *pbVar5 = bVar3;
            if ((pbVar16 <= pbVar7) && (pbVar8 = pbVar7, param_2 != 0)) goto LAB_ram_0000915a;
          }
          else if (uVar17 == 0x75) {
LAB_ram_00009440:
            for (; uVar9 != 0; uVar9 = uVar9 - 1 & 0xffff) {
              pbVar8 = pbVar7 + 1;
              uVar17 = uVar9 - 1 & 0xff;
              uVar15 = 1;
              while (uVar17 = uVar17 - 1, uVar17 != 0xffffffff) {
                uVar15 = uVar15 * 10;
              }
              *pbVar7 = (char)((uVar20 / uVar15) % 10) + 0x30;
              if ((pbVar16 <= pbVar8) && (param_2 != 0)) goto LAB_ram_0000915a;
              pbVar7 = pbVar8;
            }
          }
          else if (uVar17 == 0x78) {
LAB_ram_00009650:
            while ((uVar9 & 0xffff) != 0) {
              uVar9 = (uVar9 & 0xffff) - 1;
              iVar21 = (int)(uVar9 * 0x1000000) >> 0x18;
              uVar15 = 1;
              uVar12 = 1;
              if (uVar17 == 0x78) {
                while (iVar21 = iVar21 + -1, iVar21 != -1) {
                  uVar15 = uVar15 << 4;
                }
                uVar15 = uVar20 / uVar15 & 0xf;
                iVar21 = 0x57;
                if (uVar15 < 10) {
                  iVar21 = 0x30;
                }
                bVar11 = (byte)((iVar21 + uVar15) * 0x1000000 >> 0x18);
              }
              else {
                while (iVar21 = iVar21 + -1, iVar21 != -1) {
                  uVar12 = uVar12 << 4;
                }
                uVar15 = uVar20 / uVar12 & 0xf;
                cVar10 = '7';
                if (uVar15 < 10) {
                  cVar10 = '0';
                }
                bVar11 = cVar10 + (char)uVar15;
              }
              pbVar8 = pbVar7 + 1;
              *pbVar7 = bVar11;
              pbVar7 = pbVar8;
              if ((pbVar16 <= pbVar8) && (param_2 != 0)) goto LAB_ram_0000915a;
            }
          }
          else {
            if (uVar17 != 0x73) goto LAB_ram_00009148;
LAB_ram_0000940a:
            for (; *pbStack_68 != 0; pbStack_68 = pbStack_68 + 1) {
              if ((pbVar16 <= pbVar7) && (pbVar8 = pbVar7, param_2 != 0)) goto LAB_ram_0000915a;
              *pbVar7 = *pbStack_68;
              pbVar7 = pbVar7 + 1;
            }
          }
        }
        param_1 = pbVar7;
        if (!bVar4) goto LAB_ram_00009148;
      }
      param_1 = pbVar7;
      if (uVar6 + uVar13 < uStack_6c) {
        while ((int)((int)param_1 - (int)pbVar7 & 0xffffU) < (int)(uStack_6c - (uVar6 + uVar13))) {
          pbVar8 = param_1 + 1;
          *param_1 = 0x20;
          param_1 = pbVar8;
          if ((pbVar16 <= pbVar8) && (param_2 != 0)) goto LAB_ram_0000915a;
        }
      }
      goto LAB_ram_00009148;
    }
    if ((int)uVar17 < 0x67) {
      if (uVar17 == 99) {
        bVar3 = (byte)*param_4;
        uVar13 = 0;
        param_4 = param_4 + 1;
        uVar6 = 1;
      }
      else {
        if (uVar17 != 100) {
          if (uVar17 != 0x58) goto LAB_ram_00009292;
          goto LAB_ram_0000924c;
        }
        uVar20 = *param_4;
        param_4 = param_4 + 1;
        bVar1 = (int)uVar20 < 0;
        if (bVar1) {
          uVar20 = -uVar20;
        }
        uVar13 = (uint)bVar1;
        uVar6 = FUN_ram_000090e6(uVar20);
        uVar6 = uVar6 & 0xff;
      }
      goto LAB_ram_000092a8;
    }
    if ((uVar17 == 0x75) || (uVar17 == 0x78)) {
LAB_ram_0000924c:
      uVar20 = *param_4;
      if (uVar17 == 0x75) {
        uVar6 = FUN_ram_000090e6();
      }
      else {
        uVar6 = FUN_ram_000090fc(uVar20);
      }
LAB_ram_0000931c:
      param_4 = param_4 + 1;
      uVar6 = uVar6 & 0xff;
      uVar13 = 0;
      goto LAB_ram_000092a8;
    }
    if (uVar17 == 0x73) {
      pbStack_68 = (byte *)*param_4;
      uVar6 = strlen((char *)pbStack_68);
      goto LAB_ram_0000931c;
    }
LAB_ram_00009292:
    *pbVar7 = (byte)uVar17;
  }
  else {
LAB_ram_0000916e:
    *pbVar7 = bVar11;
  }
  param_1 = pbVar7 + 1;
  param_3 = pbVar5;
  if ((pbVar16 <= param_1) && (pbVar8 = param_1, param_2 != 0)) {
LAB_ram_0000915a:
    wch_restore_12((int)pbVar8 - (int)DAT_ram_200001f0);
    return;
  }
  goto LAB_ram_00009148;
}

