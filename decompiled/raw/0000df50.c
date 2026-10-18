/* Original address: 0x0000df50; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0000df50(undefined4 param_1,uint *param_2,undefined1 param_3,int param_4)

{
  uint uVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint auStack_44 [4];
  
  gp = &__global_pointer;
  iVar3 = disk_ioctl(param_1,1,auStack_44);
  if (iVar3 == 0) {
    iVar3 = 5;
    uVar5 = 8;
    do {
      if (auStack_44[0] / uVar5 < 0xfc3f) goto LAB_ram_0000df7e;
      iVar3 = iVar3 + -1;
      uVar5 = (uVar5 & 0x7f) << 1;
    } while (iVar3 != 0);
    uVar5 = 0xff;
LAB_ram_0000df7e:
    memset_recovered(param_4,0,0x1000);
    iVar3 = param_4 + 0x1c6;
    uVar1 = 0x3f;
    do {
      if (auStack_44[0] <= uVar1) break;
      uVar6 = *param_2;
      uVar7 = uVar6;
      if ((uVar6 < 0x65) && (uVar7 = auStack_44[0], uVar6 != 100)) {
        uVar7 = uVar6 * (auStack_44[0] / 100);
      }
      if ((auStack_44[0] < uVar7 + uVar1) || (uVar7 + uVar1 < uVar1)) {
        uVar7 = auStack_44[0] - uVar1;
      }
      else if (uVar7 == 0) break;
      FUN_ram_0000de70(iVar3,uVar1);
      FUN_ram_0000de70(iVar3 + 4,uVar7);
      *(undefined1 *)(iVar3 + -4) = param_3;
      uVar6 = (uVar1 / 0x3f) / uVar5;
      *(char *)(iVar3 + -5) = (char)uVar6;
      uVar4 = uVar1 % 0x3f;
      *(char *)(iVar3 + -7) = (char)((uVar1 / 0x3f) % uVar5);
      uVar1 = uVar1 + uVar7;
      *(byte *)(iVar3 + -6) = (byte)(uVar6 >> 2) & 0xc0 | (char)uVar4 + 1U;
      uVar6 = (uVar1 - 1) / 0x3f;
      uVar7 = uVar6 / uVar5;
      *(char *)(iVar3 + -1) = (char)uVar7;
      *(char *)(iVar3 + -3) = (char)(uVar6 % uVar5);
      *(byte *)(iVar3 + -2) = (char)((uVar1 - 1) % 0x3f) + 1U | (byte)(uVar7 >> 2) & 0xc0;
      if (param_4 + 0x1f6 == iVar3) break;
      iVar3 = iVar3 + 0x10;
      param_2 = param_2 + 1;
    } while (uVar1 != 0);
    *(undefined1 *)(param_4 + 0x1fe) = 0x55;
    *(undefined1 *)(param_4 + 0x1ff) = 0xaa;
    iVar3 = disk_write(param_1,param_4,0,1);
    bVar2 = iVar3 != 0;
  }
  else {
    bVar2 = true;
  }
  wch_restore_12(bVar2);
  return;
}

