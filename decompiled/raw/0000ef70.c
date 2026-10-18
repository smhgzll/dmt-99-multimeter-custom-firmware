/* Original address: 0x0000ef70; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0000ef70(int *param_1,byte *param_2)

{
  short sVar1;
  byte bVar2;
  byte bVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  undefined1 *puVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  byte *apbStack_44 [4];
  
  gp = &__global_pointer;
  puVar10 = (undefined1 *)*param_1;
  for (; (*param_2 == 0x2f || (*param_2 == 0x5c)); param_2 = param_2 + 1) {
  }
  param_1[2] = 0;
  if (*param_2 < 0x20) {
    *(undefined1 *)((int)param_1 + 0x2b) = 0x80;
    iVar9 = FUN_ram_0000e7fc(param_1,0);
  }
  else {
LAB_ram_0000efa2:
    uVar4 = 0;
    iVar9 = *(int *)(*param_1 + 0x10);
    apbStack_44[0] = param_2;
    while( true ) {
      uVar5 = FUN_ram_0000e086(apbStack_44);
      if (uVar5 == 0xffffffff) goto LAB_ram_0000effc;
      if (0xffff < uVar5) {
        *(short *)(uVar4 * 2 + iVar9) = (short)(uVar5 >> 0x10);
        uVar4 = uVar4 + 1;
      }
      uVar13 = uVar5 & 0xffff;
      if (uVar13 < 0x20) goto LAB_ram_0000efe2;
      if ((uVar13 == 0x2f) || (uVar13 == 0x5c)) break;
      if (((uVar13 < 0x80) &&
          (iVar7 = FUN_ram_0001e7f4(&DAT_ram_00023d00,uVar5 & 0xffff), iVar7 != 0)) ||
         (0xfe < uVar4)) goto LAB_ram_0000effc;
      *(short *)(uVar4 * 2 + iVar9) = (short)uVar5;
      uVar4 = uVar4 + 1;
    }
    for (; (bVar2 = *apbStack_44[0], bVar2 == 0x2f || (bVar2 == 0x5c));
        apbStack_44[0] = apbStack_44[0] + 1) {
    }
    if (bVar2 < 0x20) {
LAB_ram_0000efe2:
      bVar2 = 4;
    }
    else {
      bVar2 = 0;
    }
    while ((param_2 = apbStack_44[0], uVar4 != 0 &&
           ((sVar1 = *(short *)(iVar9 + uVar4 * 2 + -2), sVar1 == 0x20 || (sVar1 == 0x2e))))) {
      uVar4 = uVar4 - 1;
    }
    *(undefined2 *)(uVar4 * 2 + iVar9) = 0;
    if (uVar4 == 0) {
LAB_ram_0000effc:
      iVar9 = 6;
      goto LAB_ram_0000f014;
    }
    uVar5 = 0;
    do {
      uVar13 = uVar5;
      sVar1 = *(short *)(uVar13 * 2 + iVar9);
      uVar5 = uVar13 + 1;
    } while (sVar1 == 0x20);
    if ((uVar13 != 0) || (sVar1 == 0x2e)) {
      bVar2 = bVar2 | 3;
    }
    do {
      if (*(short *)(uVar4 * 2 + iVar9 + -2) == 0x2e) break;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
    memset_recovered(param_1 + 8,0x20,0xb);
    uVar5 = 8;
    uVar11 = 0;
    uVar12 = 0;
    while( true ) {
      uVar14 = uVar13 + 1;
      uVar8 = (uint)*(ushort *)(uVar13 * 2 + iVar9);
      if (uVar8 == 0) break;
      uVar13 = uVar14;
      if (uVar8 == 0x20) {
LAB_ram_0000f108:
        bVar2 = bVar2 | 3;
      }
      else if (uVar8 == 0x2e) {
        if (uVar14 != uVar4) goto LAB_ram_0000f108;
LAB_ram_0000f11c:
        if (uVar5 == 0xb) {
LAB_ram_0000f126:
          bVar2 = bVar2 | 3;
          uVar5 = 0xb;
          break;
        }
LAB_ram_0000f190:
        uVar11 = (uVar11 & 0x3f) << 2;
        uVar5 = 0xb;
        uVar13 = uVar4;
        uVar12 = 8;
      }
      else {
        if (uVar5 <= uVar12) {
          if (uVar5 == 0xb) goto LAB_ram_0000f126;
          if ((uVar14 == uVar4) || (bVar2 = bVar2 | 3, uVar14 <= uVar4)) goto LAB_ram_0000f190;
          break;
        }
        if (uVar14 == uVar4) goto LAB_ram_0000f11c;
        bVar3 = bVar2;
        if (uVar8 < 0x80) {
LAB_ram_0000f1f6:
          iVar7 = FUN_ram_0001e7f4("+,;=[]",uVar8);
          if (iVar7 == 0) {
            if ((uVar8 - 0x41 & 0xffff) < 0x1a) {
              uVar11 = uVar11 | 2;
            }
            else if ((uVar8 - 0x61 & 0xffff) < 0x1a) {
              uVar11 = uVar11 | 1;
              uVar8 = uVar8 - 0x20 & 0xffff;
            }
          }
          else {
LAB_ram_0000f20a:
            bVar3 = bVar3 | 3;
            uVar8 = 0x5f;
          }
LAB_ram_0000f1e6:
          *(char *)((int)param_1 + uVar12 + 0x20) = (char)uVar8;
          uVar12 = uVar12 + 1;
          bVar2 = bVar3;
        }
        else {
          bVar3 = bVar2 | 2;
          uVar6 = FUN_ram_0000fd46(uVar8);
          uVar8 = FUN_ram_0000fc9e(uVar6,0x3a8);
          if (uVar8 < 0x100) {
            if (uVar8 != 0) goto LAB_ram_0000f1f6;
            goto LAB_ram_0000f20a;
          }
          if (uVar12 < uVar5 - 1) {
            *(char *)((int)param_1 + uVar12 + 0x20) = (char)(uVar8 >> 8);
            uVar12 = uVar12 + 1;
            goto LAB_ram_0000f1e6;
          }
          uVar12 = uVar5;
          bVar2 = bVar2 | 3;
        }
      }
    }
    if ((char)param_1[8] == -0x1b) {
      *(undefined1 *)(param_1 + 8) = 5;
    }
    if (uVar5 == 8) {
      uVar11 = (uVar11 & 0x3f) << 2;
    }
    if (((uVar11 & 0xc) == 0xc) || ((uVar11 & 3) == 3)) {
      bVar2 = bVar2 | 2;
    }
    else if ((bVar2 & 2) == 0) {
      if ((uVar11 & 1) != 0) {
        bVar2 = bVar2 | 0x10;
      }
      if ((uVar11 & 4) != 0) {
        bVar2 = bVar2 | 8;
      }
    }
    *(byte *)((int)param_1 + 0x2b) = bVar2;
    iVar9 = FUN_ram_0000edde(param_1);
    if (iVar9 == 0) {
      if ((*(byte *)((int)param_1 + 0x2b) & 4) != 0) goto LAB_ram_0000f014;
      if ((*(byte *)((int)param_1 + 6) & 0x10) == 0) goto LAB_ram_0000f180;
      iVar9 = FUN_ram_0000e0f8(*puVar10,puVar10 + (uint)param_1[4] %
                                                  (uint)*(ushort *)(puVar10 + 0xc) + 0x38);
      param_1[2] = iVar9;
      goto LAB_ram_0000efa2;
    }
    if ((iVar9 != 4) || ((*(byte *)((int)param_1 + 0x2b) & 4) != 0)) goto LAB_ram_0000f014;
LAB_ram_0000f180:
    iVar9 = 5;
  }
LAB_ram_0000f014:
  wch_restore_12(iVar9);
  return;
}

