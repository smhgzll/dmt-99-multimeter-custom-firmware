/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000e2b8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000e2b8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

undefined4 FUN_ram_0000e2b8(undefined4 param_1,undefined4 *param_2,uint param_3)

{
  byte bVar1;
  ushort uVar2;
  ushort uVar3;
  char *pcVar4;
  uint uVar5;
  undefined *puVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  undefined4 uVar12;
  uint uVar13;
  char *pcVar14;
  char cVar15;
  int *piVar16;
  uint uVar17;
  int aiStack_40 [4];
  
  gp = &__global_pointer;
  *param_2 = 0;
  iVar7 = FUN_ram_0000dec0();
  if (iVar7 < 0) {
    return 0xb;
  }
  pcVar4 = *(char **)(&DAT_ram_20000234 + iVar7 * 4);
  if (pcVar4 == (char *)0x0) {
    return 0xc;
  }
  *param_2 = pcVar4;
  if ((*pcVar4 != '\0') && (uVar8 = disk_status(pcVar4[1]), (uVar8 & 1) == 0)) {
    if ((param_3 & 0xfe) == 0) {
      return 0;
    }
    if ((uVar8 & 4) != 0) {
      return 10;
    }
    return 0;
  }
  *pcVar4 = '\0';
  pcVar4[1] = (&DAT_ram_200001d4)[iVar7 * 2];
  uVar8 = disk_initialize();
  if ((uVar8 & 1) != 0) {
    return 3;
  }
  if (((param_3 & 0xfe) != 0) && ((uVar8 & 4) != 0)) {
    return 10;
  }
  iVar9 = disk_ioctl(pcVar4[1],2,pcVar4 + 0xc);
  if (iVar9 != 0) {
    return 1;
  }
  uVar2 = *(ushort *)(pcVar4 + 0xc);
  if (0xe00 < (ushort)(uVar2 - 0x200)) {
    return 1;
  }
  if ((uVar2 - 1 & uVar2) != 0) {
    return 1;
  }
  uVar8 = (uint)(byte)(&DAT_ram_200001d5)[iVar7 * 2];
  uVar10 = FUN_ram_0000e1bc(pcVar4,0);
  if (uVar10 == 2) {
LAB_ram_0000e37c:
    if (4 < uVar8) {
      return 0xd;
    }
    pcVar14 = pcVar4 + 0x1fe;
    piVar16 = aiStack_40;
    do {
      iVar7 = FUN_ram_0000de5a(pcVar14);
      *piVar16 = iVar7;
      pcVar14 = pcVar14 + 0x10;
      piVar16 = piVar16 + 1;
    } while (pcVar4 + 0x23e != pcVar14);
    iVar7 = 0;
    if (uVar8 != 0) {
      iVar7 = uVar8 - 1;
    }
    do {
      if (aiStack_40[iVar7] == 0) {
        uVar10 = 3;
        if (uVar8 != 0) goto LAB_ram_0000e5de;
      }
      else {
        uVar10 = FUN_ram_0000e1bc(pcVar4);
        if (uVar8 != 0) break;
        if (uVar10 < 2) goto LAB_ram_0000e3ca;
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 != 4);
  }
  else if (uVar10 < 3) {
    if (uVar8 == 0) goto LAB_ram_0000e5de;
    goto LAB_ram_0000e37c;
  }
  if (uVar10 == 4) {
    return 1;
  }
LAB_ram_0000e5de:
  if (uVar10 < 2) {
LAB_ram_0000e3ca:
    uVar2 = *(ushort *)(pcVar4 + 0xc);
    uVar8 = (uint)uVar2;
    if (uVar8 == *(ushort *)(pcVar4 + 0x43)) {
      uVar10 = (uint)*(ushort *)(pcVar4 + 0x4e);
      if (uVar10 == 0) {
        uVar10 = FUN_ram_0000de5a(pcVar4 + 0x5c);
      }
      bVar1 = pcVar4[0x48];
      *(uint *)(pcVar4 + 0x20) = uVar10;
      pcVar4[2] = bVar1;
      iVar7 = *(int *)(pcVar4 + 0x34);
      if ((bVar1 - 1 & 0xff) < 2) {
        uVar17 = (uint)(byte)pcVar4[0x45];
        *(ushort *)(pcVar4 + 10) = (ushort)(byte)pcVar4[0x45];
        if ((uVar17 != 0) && ((uVar17 - 1 & uVar17) == 0)) {
          uVar3 = *(ushort *)(pcVar4 + 0x49);
          uVar5 = (uint)(uVar2 >> 5);
          *(ushort *)(pcVar4 + 8) = uVar3;
          if ((uint)uVar3 % uVar5 == 0) {
            uVar11 = (uint)*(ushort *)(pcVar4 + 0x4b);
            if (uVar11 == 0) {
              uVar11 = FUN_ram_0000de5a(pcVar4 + 0x58);
            }
            uVar13 = (uint)*(ushort *)(pcVar4 + 0x46);
            if (uVar13 != 0) {
              iVar9 = bVar1 * uVar10;
              uVar5 = uVar3 / uVar5 + uVar13 + iVar9;
              if (((uVar5 <= uVar11) &&
                  (puVar6 = (undefined *)((uVar11 - uVar5) / uVar17), uVar17 <= uVar11 - uVar5)) &&
                 (puVar6 < (undefined *)0xffffff6)) {
                if (puVar6 < &UNK_ram_0000fff6) {
                  cVar15 = ((undefined *)0xff5 < puVar6) + '\x01';
                }
                else {
                  cVar15 = '\x03';
                }
                puVar6 = puVar6 + 2;
                *(uint *)(pcVar4 + 0x30) = uVar5 + iVar7;
                *(undefined **)(pcVar4 + 0x1c) = puVar6;
                *(int *)(pcVar4 + 0x24) = iVar7;
                *(uint *)(pcVar4 + 0x28) = uVar13 + iVar7;
                if (cVar15 == '\x03') {
                  if (uVar3 != 0 || *(short *)(pcVar4 + 0x62) != 0) {
                    return 0xd;
                  }
                  uVar12 = FUN_ram_0000de5a(pcVar4 + 100);
                  *(undefined4 *)(pcVar4 + 0x2c) = uVar12;
                  iVar9 = (int)puVar6 * 4;
                }
                else {
                  if (uVar3 == 0) {
                    return 0xd;
                  }
                  *(uint *)(pcVar4 + 0x2c) = uVar13 + iVar7 + iVar9;
                  if (cVar15 == '\x02') {
                    iVar9 = (int)puVar6 * 2;
                  }
                  else {
                    iVar9 = ((uint)puVar6 & 1) + ((uint)((int)puVar6 * 3) >> 1);
                  }
                }
                if ((iVar9 + (uVar8 - 1)) / uVar8 <= uVar10) {
                  pcVar4[0x18] = -1;
                  pcVar4[0x19] = -1;
                  pcVar4[0x1a] = -1;
                  pcVar4[0x1b] = -1;
                  pcVar4[0x14] = -1;
                  pcVar4[0x15] = -1;
                  pcVar4[0x16] = -1;
                  pcVar4[0x17] = -1;
                  pcVar4[4] = -0x80;
                  if ((((cVar15 == '\x03') && (*(short *)(pcVar4 + 0x68) == 1)) &&
                      ((iVar7 = FUN_ram_0000e17c(pcVar4,iVar7 + 1), iVar7 == 0 &&
                       ((pcVar4[4] = '\0', *(short *)(pcVar4 + 0x236) == -0x55ab &&
                        (iVar7 = FUN_ram_0000de5a(pcVar4 + 0x38), iVar7 == 0x41615252)))))) &&
                     (iVar7 = FUN_ram_0000de5a(pcVar4 + 0x21c), iVar7 == 0x61417272)) {
                    uVar12 = FUN_ram_0000de5a(pcVar4 + 0x220);
                    *(undefined4 *)(pcVar4 + 0x18) = uVar12;
                    uVar12 = FUN_ram_0000de5a(pcVar4 + 0x224);
                    *(undefined4 *)(pcVar4 + 0x14) = uVar12;
                  }
                  *pcVar4 = cVar15;
                  DAT_ram_2000023c = DAT_ram_2000023c + 1;
                  *(short *)(pcVar4 + 6) = DAT_ram_2000023c;
                  return 0;
                }
              }
            }
          }
        }
      }
    }
  }
  return 0xd;
}

