/* Original address: 0x0000f54a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

int FUN_ram_0000f54a(undefined4 *param_1,undefined4 param_2,byte param_3)

{
  undefined2 uVar1;
  ushort uVar2;
  ushort uVar3;
  undefined1 *puVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  byte bVar10;
  undefined4 auStack_274 [4];
  undefined1 *puStack_264;
  undefined1 *puStack_260;
  byte bStack_25a;
  int iStack_244;
  char cStack_235;
  undefined1 auStack_230 [512];
  
  gp = &__global_pointer;
  if (param_1 == (undefined4 *)0x0) {
    gp = &__global_pointer;
    return 9;
  }
  bVar10 = param_3 & 0x3f;
  auStack_274[0] = param_2;
  iVar5 = FUN_ram_0000e2b8(auStack_274,&puStack_264,bVar10);
  if (iVar5 != 0) goto LAB_ram_0000f61c;
  *(undefined1 **)(puStack_264 + 0x10) = auStack_230;
  puStack_260 = puStack_264;
  iVar5 = FUN_ram_0000ef70(&puStack_260,auStack_274[0]);
  if ((iVar5 == 0) && (cStack_235 < '\0')) {
    iVar5 = 6;
  }
  if ((param_3 & 0x1c) == 0) {
    if (iVar5 != 0) goto LAB_ram_0000f61c;
    if ((bStack_25a & 0x10) != 0) {
      iVar5 = 4;
      goto LAB_ram_0000f61c;
    }
    if (((param_3 & 2) != 0) && ((bStack_25a & 1) != 0)) goto LAB_ram_0000f708;
  }
  else {
    if (iVar5 == 0) {
      if ((bStack_25a & 0x11) != 0) {
LAB_ram_0000f708:
        iVar5 = 7;
        goto LAB_ram_0000f61c;
      }
      if ((param_3 & 4) != 0) {
        iVar5 = 8;
        goto LAB_ram_0000f61c;
      }
      if ((param_3 & 8) == 0) goto LAB_ram_0000f640;
    }
    else {
      if ((iVar5 != 4) || (iVar5 = FUN_ram_0000f28a(&puStack_260), iVar5 != 0))
      goto LAB_ram_0000f61c;
      bVar10 = bVar10 | 8;
    }
    uVar6 = FUN_ram_0000dd48();
    FUN_ram_0000de70(iStack_244 + 0xe,uVar6);
    FUN_ram_0000de70(iStack_244 + 0x16,uVar6);
    iVar5 = iStack_244;
    puVar4 = puStack_264;
    iVar7 = FUN_ram_0000e0f8(*puStack_264,iStack_244);
    *(undefined1 *)(iVar5 + 0xb) = 0x20;
    FUN_ram_0000e116(puVar4,iStack_244,0);
    *(undefined1 *)(iStack_244 + 0x1c) = 0;
    *(undefined1 *)(iStack_244 + 0x1d) = 0;
    *(undefined1 *)(iStack_244 + 0x1e) = 0;
    *(undefined1 *)(iStack_244 + 0x1f) = 0;
    puStack_264[3] = 1;
    if (iVar7 != 0) {
      uVar6 = *(undefined4 *)(puStack_264 + 0x34);
      iVar5 = FUN_ram_0000e9a4(&puStack_260,iVar7,0);
      if (iVar5 != 0) goto LAB_ram_0000f61c;
      iVar5 = FUN_ram_0000e17c(puStack_264,uVar6);
      *(int *)(puStack_264 + 0x14) = iVar7 + -1;
      if (iVar5 != 0) goto LAB_ram_0000f61c;
    }
  }
LAB_ram_0000f640:
  puVar4 = puStack_264;
  if ((bVar10 & 8) != 0) {
    bVar10 = bVar10 | 0x40;
  }
  uVar6 = *(undefined4 *)(puStack_264 + 0x34);
  param_1[9] = iStack_244;
  param_1[8] = uVar6;
  uVar8 = FUN_ram_0000e0f8(*puStack_264,iStack_244);
  param_1[2] = uVar8;
  uVar9 = FUN_ram_0000de5a(iStack_244 + 0x1c);
  uVar1 = *(undefined2 *)(puVar4 + 6);
  param_1[3] = uVar9;
  *(byte *)(param_1 + 4) = bVar10;
  *param_1 = puVar4;
  *(undefined2 *)(param_1 + 1) = uVar1;
  *(undefined1 *)((int)param_1 + 0x11) = 0;
  param_1[7] = 0;
  param_1[5] = 0;
  memset_recovered(param_1 + 10,0,0x1000);
  if ((bVar10 & 0x20) == 0) {
    gp = &__global_pointer;
    return 0;
  }
  if (uVar9 == 0) {
    gp = &__global_pointer;
    return 0;
  }
  uVar2 = *(ushort *)(puVar4 + 10);
  uVar3 = *(ushort *)(puVar4 + 0xc);
  param_1[5] = uVar9;
  do {
    if (uVar9 <= (uint)uVar2 * (uint)uVar3) {
      iVar5 = 0;
      goto LAB_ram_0000f6ba;
    }
    uVar9 = uVar9 - (uint)uVar2 * (uint)uVar3;
    uVar8 = FUN_ram_0000e71e(*param_1,uVar8);
    if (uVar8 < 2) {
      iVar5 = 2;
      goto LAB_ram_0000f6ba;
    }
  } while (uVar8 != 0xffffffff);
  iVar5 = 1;
LAB_ram_0000f6ba:
  param_1[6] = uVar8;
  if (iVar5 == 0) {
    uVar2 = *(ushort *)(puStack_264 + 0xc);
    if (uVar9 % (uint)uVar2 == 0) {
      gp = &__global_pointer;
      return 0;
    }
    iVar7 = FUN_ram_0000de84(puStack_264,uVar8);
    iVar5 = 2;
    if (iVar7 != 0) {
      iVar7 = iVar7 + uVar9 / uVar2;
      param_1[7] = iVar7;
      iVar5 = disk_read(puStack_264[1],param_1 + 10,iVar7,1);
      if (iVar5 == 0) {
        gp = &__global_pointer;
        return 0;
      }
      iVar5 = 1;
    }
  }
LAB_ram_0000f61c:
  *param_1 = 0;
  return iVar5;
}

