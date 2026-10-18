/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000ec3e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ec3e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

int FUN_ram_0000ec3e(int *param_1,uint param_2)

{
  byte bVar1;
  short sVar2;
  uint uVar3;
  byte *pbVar4;
  int iVar5;
  byte bVar6;
  byte bVar7;
  uint uVar8;
  short sVar9;
  int iVar10;
  byte bVar11;
  int iVar12;
  
  gp = &__global_pointer;
  iVar10 = *param_1;
  uVar3 = 0xff;
  bVar11 = 0xff;
  iVar12 = 4;
  while( true ) {
    if (param_1[6] == 0) {
      if (iVar12 == 0) {
        return 0;
      }
      goto LAB_ram_0000ed58;
    }
    iVar12 = FUN_ram_0000e17c(iVar10);
    if (iVar12 != 0) goto LAB_ram_0000ed58;
    pbVar4 = (byte *)param_1[7];
    bVar6 = *pbVar4;
    if (bVar6 == 0) break;
    bVar1 = pbVar4[0xb];
    bVar7 = bVar1 & 0x3f;
    *(byte *)((int)param_1 + 6) = bVar7;
    if (((bVar6 == 0xe5) || (bVar6 == 0x2e)) || (((bVar1 & 0x1f) == 8) != param_2)) {
LAB_ram_0000ed62:
      bVar11 = 0xff;
    }
    else {
      if (bVar7 != 0xf) {
        if ((bVar11 == 0) && (uVar8 = FUN_ram_0000de9e(), uVar8 == uVar3)) {
          return 0;
        }
        param_1[0xb] = -1;
        return 0;
      }
      if ((bVar6 & 0x40) == 0) {
        if (bVar11 != bVar6) goto LAB_ram_0000ed62;
      }
      else {
        uVar3 = (uint)pbVar4[0xd];
        bVar6 = bVar6 & 0xbf;
        param_1[0xb] = param_1[4];
      }
      bVar11 = 0xff;
      if ((pbVar4[0xd] == uVar3) && (*(short *)(pbVar4 + 0x1a) == 0)) {
        iVar5 = *(int *)(iVar10 + 0x10);
        sVar9 = 1;
        uVar8 = ((*pbVar4 & 0xffffffbf) - 1) * 0xd;
        iVar12 = 0;
        do {
          sVar2 = *(short *)(pbVar4 + (byte)(&DAT_ram_00023cd0)[iVar12]);
          if (sVar9 == 0) {
            if (sVar2 != -1) goto LAB_ram_0000ed62;
          }
          else {
            if (0xff < uVar8) goto LAB_ram_0000ed62;
            *(short *)(uVar8 * 2 + iVar5) = sVar2;
            uVar8 = uVar8 + 1;
            sVar9 = sVar2;
          }
          iVar12 = iVar12 + 1;
        } while (iVar12 != 0xd);
        if (((*pbVar4 & 0x40) != 0) && (sVar9 != 0)) {
          bVar11 = 0xff;
          if (0xff < uVar8) goto LAB_ram_0000ed4c;
          *(undefined2 *)(uVar8 * 2 + iVar5) = 0;
        }
        bVar11 = bVar6 - 1;
      }
    }
LAB_ram_0000ed4c:
    iVar12 = FUN_ram_0000eb26(param_1,0);
    if (iVar12 != 0) {
LAB_ram_0000ed58:
      param_1[6] = 0;
      return iVar12;
    }
  }
  iVar12 = 4;
  goto LAB_ram_0000ed58;
}

