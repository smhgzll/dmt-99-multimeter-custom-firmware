/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00008266; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00008266; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00008266(uint param_1,uint param_2,char param_3,uint param_4,undefined4 param_5,
                     int param_6)

{
  undefined *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  byte bVar7;
  uint uVar8;
  
  gp = &__global_pointer;
  uVar2 = ((uint)((param_4 & 7) != 0) + (param_4 >> 3)) * (param_4 >> 1);
  uVar3 = (uint)(byte)(param_3 - 0x20);
  if (param_4 == 0x10) {
    if ((byte)(param_3 + 0xa8U) < 3) {
      if (uVar3 == 0x38) {
        puVar1 = &UNK_ram_0001f820;
      }
      else if (uVar3 == 0x39) {
        puVar1 = &UNK_ram_0001f840;
      }
      else if (uVar3 == 0x3a) {
        puVar1 = &UNK_ram_0001f860;
      }
      else {
        puVar1 = (undefined *)0x0;
        if (uVar3 == 0x2a) goto LAB_ram_00008362;
      }
    }
    else {
      if (uVar3 != 0x2a) {
        iVar4 = uVar3 << 4;
        puVar1 = &DAT_ram_0001f230;
        goto LAB_ram_000082d8;
      }
LAB_ram_00008362:
      puVar1 = &DAT_ram_0001f880;
    }
    uVar2 = 0x20;
  }
  else {
    if (param_4 < 0x11) {
      if (param_4 != 0xc) goto LAB_ram_000082a2;
      iVar4 = uVar3 * 0xc;
      puVar1 = &DAT_ram_0001edbc;
    }
    else {
      if (param_4 != 0x18) {
        if (param_4 != 0x20) goto LAB_ram_000082a2;
        puVar1 = &DAT_ram_000205fc + uVar3 * 0x40;
        if (uVar3 == 0xe) {
          uVar2 = uVar2 >> 1;
        }
        goto LAB_ram_000082da;
      }
      iVar4 = uVar3 * 0x24;
      puVar1 = &DAT_ram_0001f8a0;
    }
LAB_ram_000082d8:
    puVar1 = puVar1 + iVar4;
  }
LAB_ram_000082da:
  uVar3 = param_2;
  for (uVar8 = 0; (uVar8 & 0xffff) < uVar2; uVar8 = uVar8 + 1) {
    bVar7 = puVar1[uVar8];
    uVar6 = uVar3 + 8 & 0xffff;
    do {
      uVar5 = param_5;
      if ((bVar7 & 0x80) == 0) {
        if (param_6 == 0) {
          uVar5 = 0;
          goto LAB_ram_0000830c;
        }
      }
      else {
LAB_ram_0000830c:
        FUN_ram_00007f00(param_1,uVar3,uVar5);
      }
      uVar3 = uVar3 + 1 & 0xffff;
      bVar7 = bVar7 << 1;
      if (display_height <= uVar3) goto LAB_ram_000082a2;
      if (param_4 == uVar3 - param_2) {
        param_1 = param_1 + 1 & 0xffff;
        uVar6 = param_2;
        if (display_width <= param_1) goto LAB_ram_000082a2;
        break;
      }
    } while (uVar3 != uVar6);
    uVar3 = uVar6;
  }
LAB_ram_000082a2:
  wch_restore_12();
  return;
}

