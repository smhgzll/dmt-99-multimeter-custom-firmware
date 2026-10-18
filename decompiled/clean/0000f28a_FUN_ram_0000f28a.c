/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000f28a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000f28a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

int FUN_ram_0000f28a(int *param_1)

{
  bool bVar1;
  byte bVar2;
  ushort *puVar3;
  byte bVar4;
  int iVar5;
  char cVar6;
  uint uVar7;
  uint uVar8;
  byte *pbVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  byte bVar13;
  int iVar14;
  int *source;
  int iVar15;
  uint uVar16;
  char acStack_45 [21];
  
  gp = &__global_pointer;
  bVar4 = *(byte *)((int)param_1 + 0x2b);
  if ((bVar4 & 0xa0) != 0) {
    return 6;
  }
  iVar14 = *param_1;
  for (iVar15 = 0; *(short *)(iVar15 * 2 + *(int *)(iVar14 + 0x10)) != 0; iVar15 = iVar15 + 1) {
  }
  source = param_1 + 8;
  _memcpy(acStack_45 + 9,source,0xc);
  if ((bVar4 & 1) == 0) {
LAB_ram_0000f498:
    iVar5 = 1;
    if ((bVar4 & 2) != 0) {
      iVar5 = (iVar15 + 0xcU) / 0xd + 1;
    }
    iVar15 = FUN_ram_0000ebf0(param_1,iVar5);
    if (iVar15 != 0) {
      return iVar15;
    }
    uVar16 = iVar5 - 1;
    if (uVar16 != 0) {
      iVar15 = FUN_ram_0000e7fc(param_1,param_1[4] + uVar16 * -0x20);
      if (iVar15 != 0) {
        return iVar15;
      }
      bVar4 = FUN_ram_0000de9e(source);
      do {
        iVar15 = FUN_ram_0000e17c(iVar14,param_1[6]);
        if (iVar15 != 0) {
          return iVar15;
        }
        iVar12 = ((uVar16 & 0xff) - 1) * 0xd;
        pbVar9 = (byte *)param_1[7];
        iVar5 = *(int *)(iVar14 + 0x10);
        bVar13 = (byte)uVar16;
        pbVar9[0xd] = bVar4;
        pbVar9[0xb] = 0xf;
        pbVar9[0xc] = 0;
        pbVar9[0x1a] = 0;
        pbVar9[0x1b] = 0;
        uVar10 = 0;
        iVar15 = 0;
        do {
          if (uVar10 != 0xffff) {
            uVar10 = (uint)*(ushort *)(iVar12 * 2 + iVar5);
            iVar12 = iVar12 + 1;
          }
          bVar2 = (&DAT_ram_00023cd0)[iVar15];
          pbVar9[bVar2] = (byte)uVar10;
          (pbVar9 + bVar2)[1] = (byte)(uVar10 >> 8);
          if (uVar10 == 0) {
            uVar10 = 0xffff;
          }
          iVar15 = iVar15 + 1;
        } while (iVar15 != 0xd);
        if ((uVar10 == 0xffff) || (*(short *)(iVar12 * 2 + iVar5) == 0)) {
          bVar13 = bVar13 | 0x40;
        }
        *pbVar9 = bVar13;
        *(undefined1 *)(iVar14 + 3) = 1;
        iVar15 = FUN_ram_0000eb26(param_1,0);
        if (iVar15 != 0) {
          return iVar15;
        }
        uVar16 = uVar16 - 1;
      } while (uVar16 != 0);
    }
    iVar15 = FUN_ram_0000e17c(iVar14,param_1[6]);
    if (iVar15 != 0) {
      return iVar15;
    }
    memset_recovered(param_1[7],0,0x20);
    _memcpy((void *)param_1[7],source,0xb);
    *(byte *)(param_1[7] + 0xc) = *(byte *)((int)param_1 + 0x2b) & 0x18;
    *(undefined1 *)(iVar14 + 3) = 1;
    return 0;
  }
  *(undefined1 *)((int)param_1 + 0x2b) = 0x40;
  uVar16 = 1;
LAB_ram_0000f2d8:
  puVar3 = *(ushort **)(iVar14 + 0x10);
  _memcpy(source,acStack_45 + 9,0xb);
  uVar10 = uVar16;
  if (5 < uVar16) {
    while (uVar7 = (uint)*puVar3, uVar7 != 0) {
      puVar3 = puVar3 + 1;
      iVar5 = 0x10;
      do {
        uVar10 = uVar10 * 2 + (uVar7 & 1);
        uVar7 = uVar7 >> 1;
        if ((int)(uVar10 * 0x8000) < 0) {
          uVar10 = uVar10 ^ 0x11021;
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
    }
  }
  uVar7 = 7;
  do {
    uVar11 = uVar7;
    uVar7 = (uVar10 & 0xf) + 0x30;
    cVar6 = (char)uVar7;
    if (0x39 < uVar7) {
      cVar6 = (char)(uVar10 & 0xf) + '7';
    }
    uVar7 = uVar11 - 1;
    acStack_45[uVar11 + 1] = cVar6;
    if (uVar7 == 0) {
      uVar8 = 1;
      break;
    }
    bVar1 = 0xf < uVar10;
    uVar8 = uVar11;
    uVar10 = uVar10 >> 4;
  } while (bVar1);
  acStack_45[uVar11] = '~';
  uVar10 = 0;
  do {
    if ((uVar7 <= uVar10) || (*(char *)((int)source + uVar10) == ' ')) break;
    if ((byte)(*(char *)((int)source + uVar10) + 0x7fU) < 0x7e) {
      if (uVar8 - 2 == uVar10) break;
      uVar10 = uVar10 + 1;
    }
    uVar10 = uVar10 + 1;
  } while( true );
  do {
    cVar6 = ' ';
    if (uVar7 < 8) {
      cVar6 = acStack_45[uVar7 + 1];
      uVar7 = uVar7 + 1;
    }
    uVar11 = uVar10 + 1;
    *(char *)((int)param_1 + uVar10 + 0x20) = cVar6;
    uVar10 = uVar11;
  } while (uVar11 < 8);
  iVar5 = FUN_ram_0000edde(param_1);
  if (iVar5 != 0) {
    if (iVar5 != 4) {
      return iVar5;
    }
    *(byte *)((int)param_1 + 0x2b) = bVar4;
    goto LAB_ram_0000f498;
  }
  uVar16 = uVar16 + 1;
  if (uVar16 == 100) {
    return 7;
  }
  goto LAB_ram_0000f2d8;
}

