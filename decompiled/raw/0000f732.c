/* Original address: 0x0000f732; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0000f732(undefined4 *param_1,void *param_2,uint param_3,int *param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *source;
  uint uVar8;
  int aiStack_44 [4];
  
  gp = &__global_pointer;
  *param_4 = 0;
  uVar2 = FUN_ram_0000df10(param_1,aiStack_44);
  if ((uVar2 == 0) && (uVar2 = (uint)*(byte *)((int)param_1 + 0x11), uVar2 == 0)) {
    if ((*(byte *)(param_1 + 4) & 1) == 0) {
      uVar2 = 7;
    }
    else {
      uVar1 = param_1[3] - param_1[5];
      if (param_3 < (uint)(param_1[3] - param_1[5])) {
        uVar1 = param_3;
      }
      source = param_1 + 10;
      for (; uVar1 != 0; uVar1 = uVar1 - uVar6) {
        uVar6 = param_1[5];
        if (uVar6 % (uint)*(ushort *)(aiStack_44[0] + 0xc) == 0) {
          uVar8 = *(ushort *)(aiStack_44[0] + 10) - 1 & uVar6 / *(ushort *)(aiStack_44[0] + 0xc);
          if (uVar8 == 0) {
            if (uVar6 == 0) {
              uVar6 = param_1[2];
            }
            else {
              uVar6 = FUN_ram_0000e71e(*param_1,param_1[6]);
            }
            if (1 < uVar6) {
              if (uVar6 != 0xffffffff) {
                param_1[6] = uVar6;
                goto LAB_ram_0000f7c4;
              }
              goto LAB_ram_0000f7ba;
            }
          }
          else {
LAB_ram_0000f7c4:
            iVar5 = aiStack_44[0];
            iVar3 = FUN_ram_0000de84(aiStack_44[0],param_1[6]);
            if (iVar3 != 0) {
              iVar3 = uVar8 + iVar3;
              if (uVar1 < *(ushort *)(iVar5 + 0xc)) {
                if (param_1[7] != iVar3) {
                  if (*(char *)(param_1 + 4) < '\0') {
                    iVar5 = disk_write(*(undefined1 *)(iVar5 + 1),source,param_1[7],1);
                    if (iVar5 != 0) goto LAB_ram_0000f7ba;
                    *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) & 0x7f;
                  }
                  iVar5 = disk_read(*(undefined1 *)(aiStack_44[0] + 1),source,iVar3,1);
                  if (iVar5 != 0) goto LAB_ram_0000f7ba;
                }
                param_1[7] = iVar3;
                goto LAB_ram_0000f880;
              }
              uVar6 = uVar1 / *(ushort *)(iVar5 + 0xc);
              if ((uint)*(ushort *)(iVar5 + 10) < uVar8 + uVar6) {
                uVar6 = *(ushort *)(iVar5 + 10) - uVar8;
              }
              iVar4 = disk_read(*(undefined1 *)(iVar5 + 1),param_2,iVar3,uVar6);
              iVar5 = aiStack_44[0];
              if (iVar4 == 0) {
                if ((*(char *)(param_1 + 4) < '\0') && ((uint)(param_1[7] - iVar3) < uVar6)) {
                  _memcpy((void *)((int)param_2 +
                                  (param_1[7] - iVar3) * (uint)*(ushort *)(aiStack_44[0] + 0xc)),
                          source,(uint)*(ushort *)(aiStack_44[0] + 0xc));
                }
                uVar6 = *(ushort *)(iVar5 + 0xc) * uVar6;
                goto LAB_ram_0000f82e;
              }
LAB_ram_0000f7ba:
              *(undefined1 *)((int)param_1 + 0x11) = 1;
              uVar2 = 1;
              break;
            }
          }
          *(undefined1 *)((int)param_1 + 0x11) = 2;
          uVar2 = 2;
          break;
        }
LAB_ram_0000f880:
        uVar8 = (uint)param_1[5] % (uint)*(ushort *)(aiStack_44[0] + 0xc);
        uVar7 = *(ushort *)(aiStack_44[0] + 0xc) - uVar8;
        uVar6 = uVar1;
        if (uVar7 < uVar1) {
          uVar6 = uVar7;
        }
        _memcpy(param_2,(void *)(uVar8 + (int)source),uVar6);
LAB_ram_0000f82e:
        param_2 = (void *)((int)param_2 + uVar6);
        *param_4 = *param_4 + uVar6;
        param_1[5] = uVar6 + param_1[5];
      }
    }
  }
  wch_restore_12(uVar2);
  return;
}

