/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000084da; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000084da; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000084da(uint param_1,undefined4 param_2,uint param_3,int param_4,uint param_5,
                     byte param_6,undefined4 param_7)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  gp = &__global_pointer;
  iVar6 = 0;
  bVar1 = false;
  uVar2 = param_4 - 1U;
  do {
    uVar2 = uVar2 & 0xff;
    if (uVar2 == 0xff) {
      wch_restore_12();
      return;
    }
    uVar5 = 1;
    uVar4 = uVar2;
    while (uVar4 = uVar4 - 1 & 0xff, uVar4 != 0xff) {
      uVar5 = uVar5 * 10;
    }
    uVar5 = (param_3 / uVar5) % 10;
    if ((bVar1) || ((int)(param_4 - 1U) <= iVar6)) {
LAB_ram_0000857c:
      iVar3 = uVar5 + 0x30;
    }
    else {
      if (uVar5 != 0) {
        bVar1 = true;
        goto LAB_ram_0000857c;
      }
      if ((char)param_6 < '\0') {
        iVar3 = 0x30;
      }
      else {
        iVar3 = 0x20;
      }
    }
    FUN_ram_00008266(param_1,param_2,iVar3,param_5,param_7,param_6 & 1);
    uVar2 = uVar2 - 1;
    iVar6 = iVar6 + 1;
    param_1 = param_1 + (param_5 >> 1) & 0xffff;
  } while( true );
}

