/* Original address: 0x0000696e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */
/* medium: Called with 0x1c00 work area and input callback before pixel decoding */

void jpeg_decoder_prepare
               (undefined4 *param_1,undefined4 param_2,int param_3,undefined4 param_4,
               undefined4 param_5)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  undefined2 *puVar9;
  byte *pbVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  byte *pbVar14;
  int iVar15;
  undefined4 uVar16;
  uint uVar17;
  int iVar18;
  uint uVar19;
  uint uVar20;
  
  gp = &__global_pointer;
  uVar16 = 5;
  if (param_3 == 0) goto LAB_ram_00006b46;
  param_1[0x1d] = param_2;
  param_1[0x1b] = param_3;
  param_1[0x1c] = param_4;
  param_1[0x1e] = param_5;
  *(undefined2 *)((int)param_1 + 0x1a) = 0;
  param_1[9] = 0;
  param_1[0xd] = 0;
  param_1[0x11] = 0;
  param_1[10] = 0;
  param_1[0xe] = 0;
  param_1[0x12] = 0;
  param_1[0xb] = 0;
  param_1[0xf] = 0;
  param_1[0x13] = 0;
  param_1[0xc] = 0;
  param_1[0x10] = 0;
  param_1[0x14] = 0;
  param_1[0x15] = 0;
  param_1[0x16] = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  pbVar5 = (byte *)FUN_ram_00006838(param_1,0x200);
  param_1[2] = pbVar5;
  uVar16 = 3;
  if (pbVar5 == (byte *)0x0) goto LAB_ram_00006b46;
  iVar6 = (*(code *)param_1[0x1d])(param_1,pbVar5,2);
  if (iVar6 != 2) {
LAB_ram_000069e8:
    uVar16 = 2;
    goto LAB_ram_00006b46;
  }
  uVar17 = 2;
  if (CONCAT11(*pbVar5,pbVar5[1]) == -0x28) {
LAB_ram_00006a1e:
    iVar6 = (*(code *)param_1[0x1d])(param_1,pbVar5,4);
    if (iVar6 != 4) goto LAB_ram_000069e8;
    uVar11 = (uint)CONCAT11(pbVar5[2],pbVar5[3]);
    if ((uVar11 < 3) || (*pbVar5 != 0xff)) goto LAB_ram_00006b44;
    uVar4 = uVar11 - 2;
    uVar17 = uVar17 + uVar11 + 2;
    switch(pbVar5[1] - 0xc0) {
    case 0:
      if (uVar4 < 0x201) {
        uVar11 = (*(code *)param_1[0x1d])(param_1,pbVar5,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
        if (uVar11 != uVar4) goto LAB_ram_000069e8;
        param_1[7] = (uint)CONCAT11(pbVar5[3],pbVar5[4]);
        param_1[8] = (uint)CONCAT11(pbVar5[1],pbVar5[2]);
        if (pbVar5[5] != 3) goto switchD_ram_00006a78_caseD_1;
        pbVar10 = pbVar5 + 7;
        iVar6 = 0;
        do {
          bVar1 = *pbVar10;
          if (iVar6 == 0) {
            if ((bVar1 != 0x11) && (1 < (byte)(bVar1 - 0x21))) goto switchD_ram_00006a78_caseD_1;
            *(byte *)((int)param_1 + 0xe) = bVar1 >> 4;
            *(byte *)((int)param_1 + 0xf) = bVar1 & 0xf;
          }
          else if (bVar1 != 0x11) goto switchD_ram_00006a78_caseD_1;
          if (3 < pbVar10[1]) goto switchD_ram_00006a78_caseD_1;
          *(byte *)((int)param_1 + iVar6 + 0x10) = pbVar10[1];
          iVar6 = iVar6 + 1;
          pbVar10 = pbVar10 + 3;
        } while (iVar6 != 3);
        goto LAB_ram_00006a1e;
      }
      break;
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 0xb:
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x19:
      goto switchD_ram_00006a78_caseD_1;
    case 4:
      if (uVar4 < 0x201) {
        uVar11 = (*(code *)param_1[0x1d])(param_1,pbVar5,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
        pbVar10 = pbVar5;
        if (uVar11 != uVar4) goto LAB_ram_000069e8;
        do {
          if (uVar4 < 0x11) goto LAB_ram_00006b44;
          bVar1 = *pbVar10;
          if ((bVar1 & 0xee) != 0) goto LAB_ram_00006b44;
          iVar6 = FUN_ram_00006838(param_1,0x10);
          if (iVar6 == 0) goto LAB_ram_00006b6a;
          iVar18 = (bVar1 & 0xf) * 2 + (uint)(bVar1 >> 4);
          param_1[iVar18 + 9] = iVar6;
          uVar11 = 0;
          iVar12 = 0;
          do {
            bVar2 = pbVar10[iVar12 + 1];
            pbVar14 = (byte *)(iVar6 + iVar12);
            iVar12 = iVar12 + 1;
            *pbVar14 = bVar2;
            uVar11 = uVar11 + bVar2;
          } while (iVar12 != 0x10);
          iVar12 = FUN_ram_00006838(param_1,uVar11 * 2);
          if (iVar12 == 0) goto LAB_ram_00006b6a;
          param_1[iVar18 + 0xd] = iVar12;
          uVar13 = 0;
          iVar15 = 0;
          iVar8 = 0;
          do {
            uVar3 = (uint)*(byte *)(iVar6 + iVar8);
            puVar9 = (undefined2 *)(iVar15 * 2 + iVar12);
            uVar19 = uVar3;
            uVar20 = uVar13;
            while (uVar19 = uVar19 - 1, uVar19 != 0xffffffff) {
              *puVar9 = (short)uVar20;
              puVar9 = puVar9 + 1;
              uVar20 = uVar20 + 1 & 0xffff;
            }
            iVar8 = iVar8 + 1;
            iVar15 = iVar15 + uVar3;
            uVar13 = (uVar13 + uVar3 & 0x7fff) << 1;
          } while (iVar8 != 0x10);
          if (uVar4 - 0x11 < uVar11) goto LAB_ram_00006b44;
          iVar6 = FUN_ram_00006838(param_1,uVar11);
          if (iVar6 == 0) goto LAB_ram_00006b6a;
          param_1[iVar18 + 0x11] = iVar6;
          pbVar10 = pbVar10 + 0x11;
          for (uVar13 = 0; uVar11 != uVar13; uVar13 = uVar13 + 1) {
            if ((bVar1 >> 4 == 0) && (0xb < *pbVar10)) goto LAB_ram_00006b44;
            *(byte *)(iVar6 + uVar13) = *pbVar10;
            pbVar10 = pbVar10 + 1;
          }
          uVar4 = (uVar4 - 0x11) - uVar11;
        } while (uVar4 != 0);
        goto LAB_ram_00006a1e;
      }
      break;
    default:
      goto switchD_ram_00006a78_caseD_8;
    case 0x1a:
      uVar16 = 4;
      if (0x200 < uVar4) goto LAB_ram_00006b46;
      uVar11 = (*(code *)param_1[0x1d])(param_1,pbVar5,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
      if (uVar11 == uVar4) {
        if ((param_1[7] == 0) || (param_1[8] == 0)) goto LAB_ram_00006b44;
        uVar16 = 8;
        if (*pbVar5 != 3) goto LAB_ram_00006b46;
        iVar6 = 0;
        goto LAB_ram_00006cd4;
      }
      goto LAB_ram_000069e8;
    case 0x1b:
      if (uVar4 < 0x201) {
        uVar11 = (*(code *)param_1[0x1d])(param_1,pbVar5,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
        pbVar10 = pbVar5;
        if (uVar11 != uVar4) goto LAB_ram_000069e8;
        do {
          if (uVar4 < 0x41) goto LAB_ram_00006b44;
          bVar1 = *pbVar10;
          uVar4 = uVar4 - 0x41;
          if ((bVar1 & 0xf0) != 0) goto LAB_ram_00006b44;
          iVar6 = FUN_ram_00006838(param_1,0x100);
          if (iVar6 == 0) goto LAB_ram_00006b6a;
          param_1[(bVar1 & 3) + 0x15] = iVar6;
          iVar12 = 0;
          do {
            pbVar14 = &DAT_ram_0001ed30 + iVar12;
            iVar18 = iVar12 + 1;
            iVar12 = iVar12 + 1;
            *(uint *)((uint)*pbVar14 * 4 + iVar6) =
                 (uint)pbVar10[iVar18] * (uint)*(ushort *)(&DAT_ram_0001ecb0 + (uint)*pbVar14 * 2);
          } while (iVar12 != 0x40);
          pbVar10 = pbVar10 + 0x41;
        } while (uVar4 != 0);
        goto LAB_ram_00006a1e;
      }
      break;
    case 0x1d:
      if (uVar4 < 0x201) {
        uVar11 = (*(code *)param_1[0x1d])(param_1,pbVar5,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
        if (uVar11 != uVar4) goto LAB_ram_000069e8;
        *(ushort *)((int)param_1 + 0x1a) = CONCAT11(*pbVar5,pbVar5[1]);
        goto LAB_ram_00006a1e;
      }
    }
    uVar16 = 4;
    goto LAB_ram_00006b46;
  }
  goto LAB_ram_00006b44;
switchD_ram_00006a78_caseD_8:
  uVar11 = (*(code *)param_1[0x1d])(param_1,0,uVar4,0xff,0x1d,(code *)param_1[0x1d]);
  if (uVar11 != uVar4) goto LAB_ram_000069e8;
  goto LAB_ram_00006a1e;
switchD_ram_00006a78_caseD_1:
  uVar16 = 8;
  goto LAB_ram_00006b46;
  while( true ) {
    if ((param_1[((iVar6 != 0) + 4) * 2 + 1] == 0) ||
       ((param_1[(uint)(iVar6 != 0) * 2 + 10] == 0 ||
        (param_1[*(byte *)((int)param_1 + iVar6 + 0x10) + 0x15] == 0)))) goto LAB_ram_00006b44;
    iVar6 = iVar12;
    if (iVar12 == 3) break;
LAB_ram_00006cd4:
    iVar12 = iVar6 + 1;
    if ((pbVar5[iVar12 * 2] != 0) && (pbVar5[iVar12 * 2] != 0x11))
    goto switchD_ram_00006a78_caseD_1;
  }
  iVar6 = (uint)*(byte *)((int)param_1 + 0xf) * (uint)*(byte *)((int)param_1 + 0xe);
  if (iVar6 != 0) {
    uVar11 = iVar6 * 0x80 + 0x40;
    if (uVar11 < 0x100) {
      uVar11 = 0x100;
    }
    iVar12 = FUN_ram_00006838(param_1,uVar11);
    param_1[0x19] = iVar12;
    if (iVar12 != 0) {
      iVar6 = FUN_ram_00006838(param_1,(iVar6 + 2) * 0x40);
      param_1[0x1a] = iVar6;
      if (iVar6 != 0) {
        param_1[1] = pbVar5;
        *param_1 = 0;
        *(undefined1 *)(param_1 + 3) = 0;
        uVar17 = uVar17 & 0x1ff;
        uVar16 = 0;
        if (uVar17 != 0) {
          uVar7 = (*(code *)param_1[0x1d])(param_1,pbVar5 + uVar17,0x200 - uVar17);
          *param_1 = uVar7;
          param_1[1] = pbVar5 + (uVar17 - 1);
        }
        goto LAB_ram_00006b46;
      }
    }
LAB_ram_00006b6a:
    uVar16 = 3;
    goto LAB_ram_00006b46;
  }
LAB_ram_00006b44:
  uVar16 = 6;
LAB_ram_00006b46:
  wch_restore_12(uVar16);
  return;
}

