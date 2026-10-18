/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000f962; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000f962; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

uint FUN_ram_0000f962(undefined4 *param_1,uint param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iStack_24;
  
  gp = &__global_pointer;
  uVar2 = FUN_ram_0000df10(param_1,&iStack_24);
  if (uVar2 != 0) {
    return uVar2;
  }
  if (*(byte *)((int)param_1 + 0x11) != 0) {
    return (uint)*(byte *)((int)param_1 + 0x11);
  }
  if (((uint)param_1[3] < param_2) && ((*(byte *)(param_1 + 4) & 2) == 0)) {
    param_2 = param_1[3];
  }
  iVar4 = param_1[5];
  param_1[5] = 0;
  if (param_2 == 0) {
LAB_ram_0000f994:
    iVar4 = 0;
  }
  else {
    uVar2 = (uint)*(ushort *)(iStack_24 + 10) * (uint)*(ushort *)(iStack_24 + 0xc);
    if ((iVar4 == 0) || (uVar5 = iVar4 - 1, (param_2 - 1) / uVar2 < uVar5 / uVar2)) {
      iVar4 = param_1[2];
      if (iVar4 == 0) {
        iVar4 = FUN_ram_0000e8b2(param_1);
        if (iVar4 == 1) goto LAB_ram_0000fa56;
        if (iVar4 == -1) goto LAB_ram_0000fa6a;
        param_1[2] = iVar4;
      }
      param_1[6] = iVar4;
    }
    else {
      iVar4 = param_1[6];
      uVar5 = uVar5 & -uVar2;
      param_1[5] = uVar5;
      param_2 = param_2 - uVar5;
    }
    if (iVar4 == 0) goto LAB_ram_0000f994;
    while (uVar2 < param_2) {
      param_1[5] = param_1[5] + uVar2;
      param_2 = param_2 - uVar2;
      if ((*(byte *)(param_1 + 4) & 2) == 0) {
        uVar5 = FUN_ram_0000e71e(*param_1);
      }
      else {
        uVar5 = FUN_ram_0000e8b2(param_1);
        if (uVar5 == 0) {
          param_2 = 0;
          break;
        }
      }
      if (uVar5 == 0xffffffff) goto LAB_ram_0000fa6a;
      if ((uVar5 < 2) || (*(uint *)(iStack_24 + 0x1c) <= uVar5)) goto LAB_ram_0000fa56;
      param_1[6] = uVar5;
    }
    uVar1 = *(ushort *)(iStack_24 + 0xc);
    param_1[5] = param_1[5] + param_2;
    if (param_2 % (uint)uVar1 == 0) goto LAB_ram_0000f994;
    iVar4 = FUN_ram_0000de84();
    if (iVar4 == 0) {
LAB_ram_0000fa56:
      *(undefined1 *)((int)param_1 + 0x11) = 2;
      return 2;
    }
    iVar4 = param_2 / uVar1 + iVar4;
  }
  uVar2 = param_1[5];
  if ((uint)param_1[3] < uVar2) {
    param_1[3] = uVar2;
    *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) | 0x40;
  }
  if (uVar2 % (uint)*(ushort *)(iStack_24 + 0xc) == 0) {
    return 0;
  }
  if (param_1[7] == iVar4) {
    return 0;
  }
  if (*(char *)(param_1 + 4) < '\0') {
    iVar3 = disk_write(*(undefined1 *)(iStack_24 + 1),param_1 + 10,param_1[7],1);
    if (iVar3 != 0) goto LAB_ram_0000fa6a;
    *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) & 0x7f;
  }
  iVar3 = disk_read(*(undefined1 *)(iStack_24 + 1),param_1 + 10,iVar4,1);
  if (iVar3 == 0) {
    param_1[7] = iVar4;
    return 0;
  }
LAB_ram_0000fa6a:
  *(undefined1 *)((int)param_1 + 0x11) = 1;
  return 1;
}

