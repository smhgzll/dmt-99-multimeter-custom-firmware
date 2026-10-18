/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000edde; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000edde; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0000edde(int *param_1)

{
  bool bVar1;
  byte bVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  short sVar7;
  short sVar8;
  byte bVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  int iVar13;
  byte *pbVar14;
  byte bVar15;
  
  gp = &__global_pointer;
  iVar13 = *param_1;
  iVar5 = FUN_ram_0000e7fc(param_1,0);
  if (iVar5 == 0) {
    param_1[0xb] = -1;
    uVar12 = 0xff;
    bVar3 = 0xff;
    while (iVar5 = FUN_ram_0000e17c(iVar13,param_1[6]), iVar5 == 0) {
      pbVar14 = (byte *)param_1[7];
      bVar15 = *pbVar14;
      if (bVar15 == 0) {
        iVar5 = 4;
        break;
      }
      bVar2 = pbVar14[0xb];
      bVar9 = bVar2 & 0x3f;
      *(byte *)((int)param_1 + 6) = bVar9;
      if (bVar15 == 0xe5) {
LAB_ram_0000ee50:
        param_1[0xb] = -1;
LAB_ram_0000ef34:
        bVar3 = 0xff;
      }
      else {
        if ((bVar2 & 8) == 0) {
          if (bVar9 != 0xf) {
            if (((bVar3 != 0) || (uVar4 = FUN_ram_0000de9e(pbVar14), uVar4 != uVar12)) &&
               (((*(byte *)((int)param_1 + 0x2b) & 1) != 0 ||
                (iVar10 = FUN_ram_0001e57a(pbVar14,param_1 + 8,0xb), iVar10 != 0))))
            goto LAB_ram_0000ee50;
            break;
          }
        }
        else if (bVar9 != 0xf) goto LAB_ram_0000ee50;
        if ((*(byte *)((int)param_1 + 0x2b) & 0x40) == 0) {
          if ((bVar15 & 0x40) == 0) {
            if (bVar3 != bVar15) goto LAB_ram_0000ef34;
          }
          else {
            uVar12 = (uint)pbVar14[0xd];
            bVar15 = bVar15 & 0xbf;
            param_1[0xb] = param_1[4];
          }
          bVar3 = 0xff;
          if ((pbVar14[0xd] == uVar12) && (*(short *)(pbVar14 + 0x1a) == 0)) {
            iVar5 = *(int *)(iVar13 + 0x10);
            uVar4 = ((*pbVar14 & 0x3f) - 1) * 0xd;
            iVar10 = 0;
            sVar7 = 1;
            do {
              sVar8 = *(short *)(pbVar14 + (byte)(&DAT_ram_00023cd0)[iVar10]);
              if (sVar7 == 0) {
                bVar1 = sVar8 != -1;
                sVar8 = sVar7;
                if (bVar1) goto LAB_ram_0000ef34;
              }
              else {
                if (0xff < uVar4) goto LAB_ram_0000ef34;
                iVar6 = FUN_ram_0000fd46(sVar8);
                iVar11 = uVar4 * 2;
                uVar4 = uVar4 + 1;
                iVar11 = FUN_ram_0000fd46(*(undefined2 *)(iVar11 + iVar5));
                if (iVar6 != iVar11) goto LAB_ram_0000ef34;
              }
              iVar10 = iVar10 + 1;
              sVar7 = sVar8;
            } while (iVar10 != 0xd);
            if ((((*pbVar14 & 0x40) == 0) || (sVar8 == 0)) ||
               (bVar3 = 0xff, *(short *)(uVar4 * 2 + iVar5) == 0)) {
              bVar3 = bVar15 - 1;
            }
          }
        }
      }
      iVar5 = FUN_ram_0000eb26(param_1,0);
      if (iVar5 != 0) break;
    }
  }
  wch_restore_12(iVar5);
  return;
}

