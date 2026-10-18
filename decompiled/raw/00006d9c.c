/* Original address: 0x00006d9c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */
/* medium: Decoder calls rectangle callback; JPEG preparation relationship */

undefined4 jpeg_decoder_decompress(int *param_1,code *param_2,uint param_3)

{
  bool bVar1;
  char cVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  byte bVar6;
  char cVar7;
  char cVar8;
  byte bVar9;
  undefined1 uVar10;
  undefined1 *puVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  undefined4 uVar15;
  uint uVar16;
  byte *pbVar17;
  uint uVar18;
  byte *pbVar19;
  undefined1 *puVar20;
  int iVar21;
  byte *pbVar22;
  undefined1 *puVar23;
  undefined1 *puVar24;
  undefined1 *puVar25;
  uint uVar26;
  uint extraout_a3;
  int iVar27;
  uint uVar28;
  int *piVar29;
  int *piVar30;
  char *pcVar31;
  int iVar32;
  uint uVar33;
  byte *pbVar34;
  uint uVar35;
  uint uVar36;
  char *pcVar37;
  int *piVar38;
  uint uVar39;
  int *piVar40;
  int iVar41;
  int iVar42;
  int iVar43;
  int iVar44;
  uint uVar45;
  int iVar46;
  uint uVar47;
  uint uVar48;
  uint uStack_7c;
  uint uStack_78;
  uint uStack_74;
  short sStack_48;
  short sStack_46;
  short sStack_44;
  short sStack_42;
  
  gp = &__global_pointer;
  if (3 < param_3) {
    return 5;
  }
  bVar5 = *(byte *)((int)param_1 + 0xe);
  bVar6 = *(byte *)((int)param_1 + 0xf);
  *(char *)((int)param_1 + 0xd) = (char)param_3;
  param_1[5] = 0;
  *(undefined2 *)(param_1 + 6) = 0;
  uStack_74 = 0;
  uStack_78 = 0;
  uVar33 = 0;
  do {
    if ((uint)param_1[8] <= uVar33) {
      iVar13 = 0;
LAB_ram_00006e54:
      uVar15 = wch_restore_12(iVar13);
      return uVar15;
    }
    for (uStack_7c = 0; uStack_7c < (uint)param_1[7]; uStack_7c = uStack_7c + (uint)bVar5 * 8) {
      uVar26 = (uint)*(ushort *)((int)param_1 + 0x1a);
      if ((uVar26 != 0) && (bVar1 = uVar26 == uStack_78, uStack_78 = uStack_78 + 1 & 0xffff, bVar1))
      {
        pbVar34 = (byte *)param_1[1];
        iVar14 = *param_1;
        iVar13 = 2;
        uVar35 = 0;
        while( true ) {
          if (iVar14 == 0) {
            pbVar34 = (byte *)param_1[2];
            iVar14 = (*(code *)param_1[0x1d])(param_1,pbVar34,0x200,uVar26,(code *)param_1[0x1d]);
            uVar26 = extraout_a3;
            if (iVar14 == 0) {
              iVar13 = 2;
              goto LAB_ram_00006e54;
            }
          }
          else {
            pbVar34 = pbVar34 + 1;
          }
          uVar28 = (uint)*pbVar34;
          iVar14 = iVar14 + -1;
          if (iVar13 == 1) break;
          iVar13 = 1;
          uVar35 = uVar28;
        }
        param_1[1] = (int)pbVar34;
        *param_1 = iVar14;
        *(undefined1 *)(param_1 + 3) = 0;
        if (((undefined *)(uVar35 << 8 | uVar28 & 0xffd8) != &DAT_ram_0000ffd0) ||
           (((uStack_74 ^ uVar28) & 7) != 0)) {
LAB_ram_00006e52:
          iVar13 = 6;
          goto LAB_ram_00006e54;
        }
        uStack_74 = uStack_74 + 1 & 0xffff;
        param_1[5] = 0;
        *(undefined2 *)(param_1 + 6) = 0;
        uStack_78 = 1;
      }
      piVar38 = (int *)param_1[0x19];
      uVar35 = (uint)*(byte *)((int)param_1 + 0xe) * (uint)*(byte *)((int)param_1 + 0xf);
      pcVar37 = (char *)param_1[0x1a];
      uVar26 = 0;
      do {
        iVar14 = 0;
        if (uVar35 <= uVar26) {
          iVar14 = (1 - uVar35) + uVar26;
        }
        uVar28 = (uint)(iVar14 != 0);
        iVar13 = FUN_ram_000068d2(param_1,param_1[uVar28 * 2 + 9],param_1[uVar28 * 2 + 0xd],
                                  param_1[uVar28 * 2 + 0x11]);
        if (iVar13 < 0) {
          iVar13 = -iVar13;
          goto LAB_ram_00006e54;
        }
        iVar41 = (int)*(short *)((int)param_1 + iVar14 * 2 + 0x14);
        if (iVar13 != 0) {
          uVar16 = FUN_ram_00006854(param_1,iVar13);
          if ((int)uVar16 < 0) {
LAB_ram_00006ee8:
            iVar13 = -uVar16;
            goto LAB_ram_00006e54;
          }
          uVar39 = 1 << (iVar13 - 1U & 0x1f);
          if ((uVar16 & uVar39) == 0) {
            uVar16 = uVar16 - (uVar39 * 2 + -1);
          }
          iVar41 = iVar41 + uVar16;
          *(short *)((int)param_1 + iVar14 * 2 + 0x14) = (short)iVar41;
        }
        piVar40 = (int *)param_1[*(byte *)((int)param_1 + iVar14 + 0x10) + 0x15];
        piVar29 = piVar38 + 1;
        *piVar38 = iVar41 * *piVar40 >> 8;
        do {
          *piVar29 = 0;
          piVar29 = piVar29 + 1;
        } while (piVar38 + 0x40 != piVar29);
        iVar13 = param_1[uVar28 * 2 + 10];
        iVar14 = param_1[uVar28 * 2 + 0xe];
        iVar41 = param_1[uVar28 * 2 + 0x12];
        uVar28 = 1;
        do {
          uVar16 = FUN_ram_000068d2(param_1,iVar13,iVar14,iVar41);
          if (uVar16 == 0) break;
          if ((int)uVar16 < 0) goto LAB_ram_00006ee8;
          if ((uVar16 >> 4 != 0) && (uVar28 = uVar28 + (uVar16 >> 4), 0x3f < uVar28))
          goto LAB_ram_00006e52;
          uVar39 = uVar16 & 0xf;
          if (uVar39 != 0) {
            uVar16 = FUN_ram_00006854(param_1,uVar39);
            if ((int)uVar16 < 0) goto LAB_ram_00006ee8;
            uVar39 = 1 << (uVar39 - 1 & 0x1f);
            if ((uVar16 & uVar39) == 0) {
              uVar16 = uVar16 - (uVar39 * 2 + -1);
            }
            piVar38[(byte)(&DAT_ram_0001ed30)[uVar28]] =
                 (int)(uVar16 * piVar40[(byte)(&DAT_ram_0001ed30)[uVar28]]) >> 8;
          }
          uVar28 = uVar28 + 1;
        } while (uVar28 != 0x40);
        if (*(char *)((int)param_1 + 0xd) == '\x03') {
          *pcVar37 = (char)(*piVar38 / 0x100) + -0x80;
        }
        else {
          piVar29 = piVar38;
          do {
            iVar13 = *piVar29 - piVar29[0x20];
            iVar43 = *piVar29 + piVar29[0x20];
            iVar14 = piVar29[0x10] + piVar29[0x30];
            iVar46 = iVar43 + iVar14;
            iVar43 = iVar43 - iVar14;
            piVar30 = piVar29 + 1;
            iVar14 = ((piVar29[0x10] - piVar29[0x30]) * 0x16a0 >> 0xc) - iVar14;
            iVar44 = iVar13 + iVar14;
            iVar13 = iVar13 - iVar14;
            iVar14 = piVar29[8] - piVar29[0x38];
            iVar41 = piVar29[8] + piVar29[0x38];
            iVar42 = piVar29[0x28] - piVar29[0x18];
            iVar27 = piVar29[0x18] + piVar29[0x28];
            iVar32 = iVar41 + iVar27;
            iVar21 = (iVar14 + iVar42) * 0x1d90 >> 0xc;
            iVar42 = (iVar21 - (iVar42 * 0x29cf >> 0xc)) - iVar32;
            *piVar29 = iVar46 + iVar32;
            piVar29[0x38] = iVar46 - iVar32;
            piVar29[8] = iVar44 + iVar42;
            iVar41 = ((iVar41 - iVar27) * 0x16a0 >> 0xc) - iVar42;
            iVar14 = (iVar21 - (iVar14 * 0x1151 >> 0xc)) - iVar41;
            piVar29[0x30] = iVar44 - iVar42;
            piVar29[0x10] = iVar13 + iVar41;
            piVar29[0x28] = iVar13 - iVar41;
            piVar29[0x18] = iVar43 + iVar14;
            piVar29[0x20] = iVar43 - iVar14;
            piVar40 = piVar38;
            piVar29 = piVar30;
            pcVar31 = pcVar37;
          } while (piVar30 != piVar38 + 8);
          do {
            iVar13 = *piVar40 + 0x8000 + piVar40[4];
            iVar41 = (*piVar40 + 0x8000) - piVar40[4];
            iVar14 = piVar40[6] + piVar40[2];
            iVar44 = iVar13 + iVar14;
            piVar29 = piVar40 + 8;
            iVar21 = ((piVar40[2] - piVar40[6]) * 0x16a0 >> 0xc) - iVar14;
            iVar32 = iVar41 + iVar21;
            iVar41 = iVar41 - iVar21;
            iVar13 = iVar13 - iVar14;
            iVar14 = piVar40[1] - piVar40[7];
            iVar42 = piVar40[5] - piVar40[3];
            iVar43 = piVar40[7] + piVar40[1];
            iVar21 = piVar40[3] + piVar40[5];
            iVar46 = iVar43 + iVar21;
            iVar27 = (iVar14 + iVar42) * 0x1d90 >> 0xc;
            iVar42 = (iVar27 - (iVar42 * 0x29cf >> 0xc)) - iVar46;
            iVar21 = ((iVar43 - iVar21) * 0x16a0 >> 0xc) - iVar42;
            pcVar31[7] = (&DAT_ram_0001e8b0)[iVar44 - iVar46 >> 8 & 0x3ff];
            iVar14 = (iVar27 - (iVar14 * 0x1151 >> 0xc)) - iVar21;
            pcVar31[6] = (&DAT_ram_0001e8b0)[iVar32 - iVar42 >> 8 & 0x3ff];
            pcVar31[5] = (&DAT_ram_0001e8b0)[iVar41 - iVar21 >> 8 & 0x3ff];
            pcVar31[3] = (&DAT_ram_0001e8b0)[iVar13 + iVar14 >> 8 & 0x3ff];
            cVar2 = (&DAT_ram_0001e8b0)[iVar32 + iVar42 >> 8 & 0x3ff];
            cVar7 = (&DAT_ram_0001e8b0)[iVar41 + iVar21 >> 8 & 0x3ff];
            cVar8 = (&DAT_ram_0001e8b0)[iVar13 - iVar14 >> 8 & 0x3ff];
            *pcVar31 = (&DAT_ram_0001e8b0)[iVar44 + iVar46 >> 8 & 0x3ff];
            pcVar31[1] = cVar2;
            pcVar31[2] = cVar7;
            pcVar31[4] = cVar8;
            piVar40 = piVar29;
            pcVar31 = pcVar31 + 8;
          } while (piVar29 != piVar38 + 0x40);
        }
        uVar26 = uVar26 + 1;
        pcVar37 = pcVar37 + 0x40;
      } while (uVar26 < uVar35 + 2);
      uVar35 = (uint)*(byte *)((int)param_1 + 0xe);
      bVar3 = *(byte *)((int)param_1 + 0xf);
      uVar28 = uVar35 * 8;
      uVar16 = (uint)bVar3 * 8;
      uVar26 = param_1[7] - uStack_7c;
      if (uStack_7c + uVar28 <= (uint)param_1[7]) {
        uVar26 = uVar28;
      }
      uVar39 = uVar16;
      if ((uint)param_1[8] < uVar33 + uVar16) {
        uVar39 = param_1[8] - uVar33;
      }
      uVar18 = (uint)*(byte *)((int)param_1 + 0xd);
      uVar26 = uVar26 >> (uVar18 & 0x1f);
      if ((uVar26 != 0) && (uVar39 = uVar39 >> (uVar18 & 0x1f), uVar39 != 0)) {
        puVar11 = (undefined1 *)param_1[0x19];
        sStack_48 = (short)(uStack_7c >> (uVar18 & 0x1f));
        sStack_46 = sStack_48 + -1 + (short)uVar26;
        sStack_44 = (short)(uVar33 >> (uVar18 & 0x1f));
        sStack_42 = sStack_44 + -1 + (short)uVar39;
        if (uVar18 == 3) {
          pbVar34 = (byte *)(uVar28 * uVar16 + param_1[0x1a]);
          iVar14 = pbVar34[0x40] - 0x80;
          iVar13 = *pbVar34 - 0x80;
          for (uVar35 = 0; uVar35 < uVar16; uVar35 = uVar35 + 8) {
            iVar41 = param_1[0x1a];
            if (uVar35 == 8) {
              iVar41 = iVar41 + 0x80;
            }
            for (uVar18 = 0; uVar18 < uVar28; uVar18 = uVar18 + 8) {
              uVar12 = (uint)*(byte *)(uVar18 * 8 + iVar41);
              *puVar11 = (&DAT_ram_0001e8b0)[uVar12 + (iVar14 * 0x59b) / 0x400 & 0x3ff];
              uVar10 = (&DAT_ram_0001e8b0)[uVar12 + (iVar13 * 0x716) / 0x400 & 0x3ff];
              puVar11[1] = (&DAT_ram_0001e8b0)
                           [uVar12 - (iVar14 * 0x2db + iVar13 * 0x160) / 0x400 & 0x3ff];
              puVar11[2] = uVar10;
              puVar11 = puVar11 + 3;
            }
          }
        }
        else {
          iVar13 = uVar35 << 6;
          for (uVar18 = 0; uVar16 != uVar18; uVar18 = uVar18 + 1) {
            iVar14 = param_1[0x1a];
            pbVar22 = (byte *)(uVar18 * 8 + iVar14);
            pbVar34 = (byte *)(iVar14 + iVar13);
            if ((bVar3 == 2) &&
               (pbVar34 = (byte *)(((uVar18 >> 1) + 0x20) * 8 + iVar14), 7 < uVar18)) {
              pbVar22 = pbVar22 + 0x40;
            }
            puVar23 = puVar11;
            for (uVar12 = 0; uVar28 != uVar12; uVar12 = uVar12 + 1) {
              bVar9 = *pbVar34;
              bVar4 = pbVar34[0x40];
              if (uVar35 == 2) {
                if (uVar12 == 8) {
                  pbVar22 = pbVar22 + 0x38;
                }
                uVar36 = uVar12 & 1;
              }
              else {
                uVar36 = 1;
              }
              pbVar34 = pbVar34 + uVar36;
              uVar36 = (uint)*pbVar22;
              pbVar22 = pbVar22 + 1;
              *puVar23 = (&DAT_ram_0001e8b0)[(int)((bVar4 - 0x80) * 0x59b) / 0x400 + uVar36 & 0x3ff]
              ;
              puVar23[1] = (&DAT_ram_0001e8b0)
                           [uVar36 - (int)((bVar9 - 0x80) * 0x160 + (bVar4 - 0x80) * 0x2db) / 0x400
                            & 0x3ff];
              puVar23[2] = (&DAT_ram_0001e8b0)
                           [uVar36 + (int)((bVar9 - 0x80) * 0x716) / 0x400 & 0x3ff];
              puVar23 = puVar23 + 3;
            }
            puVar11 = puVar11 + uVar35 * 0x18;
            iVar13 = iVar13 + 8;
          }
          uVar35 = (uint)*(byte *)((int)param_1 + 0xd);
          if (uVar35 != 0) {
            iVar13 = 1 << (uVar35 & 0x1f);
            puVar11 = (undefined1 *)param_1[0x19];
            uVar12 = uVar35 << 1;
            iVar14 = 3 << (uVar35 & 0x1f);
            iVar41 = 0;
            for (uVar18 = 0; uVar18 < uVar16; uVar18 = uVar18 + iVar13) {
              iVar21 = iVar41;
              for (uVar36 = 0; uVar36 < uVar28; uVar36 = uVar36 + iVar13) {
                pbVar34 = (byte *)(param_1[0x19] + iVar21);
                uVar45 = 0;
                uVar47 = 0;
                uVar48 = 0;
                for (iVar27 = 0; iVar13 != iVar27; iVar27 = iVar27 + 1) {
                  for (pbVar22 = pbVar34; pbVar22 != pbVar34 + iVar14; pbVar22 = pbVar22 + 3) {
                    uVar48 = uVar48 + *pbVar22;
                    uVar47 = uVar47 + pbVar22[1];
                    uVar45 = uVar45 + pbVar22[2];
                  }
                  pbVar34 = pbVar34 + (uVar28 - iVar13) * 3 + iVar14;
                }
                *puVar11 = (char)(uVar48 >> (uVar12 & 0x1f));
                puVar11[1] = (char)(uVar47 >> (uVar12 & 0x1f));
                puVar11[2] = (char)(uVar45 >> (uVar12 & 0x1f));
                iVar21 = iVar21 + iVar14;
                puVar11 = puVar11 + 3;
              }
              iVar41 = iVar41 + (uVar28 << (uVar35 & 0x1f)) * 3;
            }
          }
        }
        uVar28 = uVar28 >> (*(byte *)((int)param_1 + 0xd) & 0x1f);
        if (uVar26 < uVar28) {
          puVar23 = (undefined1 *)param_1[0x19];
          uVar35 = 0;
          puVar11 = puVar23;
          do {
            puVar20 = puVar23;
            puVar24 = puVar11;
            do {
              puVar25 = puVar24 + 3;
              *puVar20 = *puVar24;
              puVar20[1] = puVar24[1];
              puVar20[2] = puVar24[2];
              puVar20 = puVar20 + 3;
              puVar24 = puVar25;
            } while (puVar11 + uVar26 * 3 != puVar25);
            uVar35 = uVar35 + 1;
            puVar23 = puVar23 + uVar26 * 3;
            puVar11 = puVar11 + uVar28 * 3;
          } while (uVar39 != uVar35);
        }
        pbVar19 = (byte *)param_1[0x19];
        pbVar34 = pbVar19;
        pbVar22 = pbVar19;
        do {
          pbVar17 = pbVar34 + 3;
          uVar35 = (pbVar34[1] & 0xfc) << 3 | (int)((*pbVar34 & 0xfffff8) << 0x18) >> 0x10 |
                   (uint)(pbVar34[2] >> 3);
          *(short *)pbVar22 = (short)uVar35;
          pbVar34 = pbVar17;
          pbVar22 = pbVar22 + 2;
        } while (pbVar17 != pbVar19 + uVar26 * uVar39 * 3);
        iVar13 = (*param_2)(param_1,pbVar19,&sStack_48,uVar35,pbVar19 + uVar26 * uVar39 * 3,param_2)
        ;
        if (iVar13 == 0) {
          iVar13 = 1;
          goto LAB_ram_00006e54;
        }
      }
    }
    uVar33 = uVar33 + (uint)bVar6 * 8;
  } while( true );
}

