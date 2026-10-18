/* Original address: 0x000083d6; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000083d6(uint param_1,uint param_2,int param_3,uint param_4,undefined4 param_5,
                     undefined4 param_6,int param_7)

{
  uint uVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  byte bVar7;
  
  gp = &__global_pointer;
  uVar1 = param_3 - 0x20U & 0xff;
  if (param_4 == 0x10) {
    iVar2 = uVar1 << 4;
    puVar3 = &DAT_ram_0001f230;
  }
  else if (param_4 < 0x11) {
    if (param_4 != 0xc) goto LAB_ram_00008416;
    iVar2 = uVar1 * 0xc;
    puVar3 = &DAT_ram_0001edbc;
  }
  else if (param_4 == 0x18) {
    iVar2 = uVar1 * 0x24;
    puVar3 = &DAT_ram_0001f8a0;
  }
  else {
    if (param_4 != 0x20) goto LAB_ram_00008416;
    iVar2 = uVar1 << 6;
    puVar3 = &DAT_ram_000205fc;
  }
  uVar1 = param_2;
  for (uVar5 = 0; (uVar5 & 0xffff) < ((uint)((param_4 & 7) != 0) + (param_4 >> 3)) * (param_4 >> 1);
      uVar5 = uVar5 + 1) {
    bVar7 = puVar3[uVar5 + iVar2];
    uVar6 = uVar1 + 8 & 0xffff;
    do {
      uVar4 = param_5;
      if (((bVar7 & 0x80) != 0) || (uVar4 = param_6, param_7 == 0)) {
        FUN_ram_00007f00(param_1,uVar1,uVar4);
      }
      uVar1 = uVar1 + 1 & 0xffff;
      bVar7 = bVar7 << 1;
      if (display_height <= uVar1) goto LAB_ram_00008416;
      if (param_4 == uVar1 - param_2) {
        param_1 = param_1 + 1 & 0xffff;
        uVar6 = param_2;
        if (display_width <= param_1) goto LAB_ram_00008416;
        break;
      }
    } while (uVar1 != uVar6);
    uVar1 = uVar6;
  }
LAB_ram_00008416:
  wch_restore_12();
  return;
}

