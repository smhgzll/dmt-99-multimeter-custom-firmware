/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00008a5a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00008a5a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00008a5a(uint param_1,uint param_2,char *param_3,undefined4 param_4,undefined4 param_5,
                     uint param_6,int param_7)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  char *pcVar8;
  uint uVar9;
  uint uVar10;
  
  gp = &__global_pointer;
  pcVar8 = &DAT_ram_00021dbc;
  iVar7 = 0;
  uVar5 = 0;
  uVar2 = (param_6 & 0xffff) - 1 & 0xffff;
  uVar10 = param_1;
  do {
    if ((*pcVar8 == *param_3) && (pcVar8[1] == param_3[1])) {
      FUN_ram_000086f6(uVar10,param_2,uVar2 + uVar10 & 0xffff,uVar2 + param_2 & 0xffff);
      for (uVar6 = 0; uVar6 < (param_6 & 0xffff) * ((uint)((param_6 & 7) != 0) + (param_6 >> 3));
          uVar6 = uVar6 + 1 & 0xff) {
        bVar1 = (&DAT_ram_00021dbe)[iVar7 * 0x1a + uVar6];
        uVar9 = 0;
        do {
          uVar4 = (int)(uint)bVar1 >> (uVar9 & 0x1f) & 1;
          if (param_7 == 0) {
            uVar3 = param_4;
            if (uVar4 == 0) {
              uVar3 = param_5;
            }
            display_write_data((ushort)uVar3);
            uVar5 = uVar5 + 1 & 0xff;
            if (uVar5 % param_6 == 0) {
              uVar5 = 0;
              break;
            }
          }
          else {
            if (uVar4 != 0) {
              FUN_ram_00007f00(uVar10,param_2,param_4);
            }
            uVar10 = uVar10 + 1 & 0xffff;
            if (uVar10 - param_1 == param_6) {
              param_2 = param_2 + 1 & 0xffff;
              uVar10 = param_1;
              break;
            }
          }
          uVar9 = uVar9 + 1;
        } while (uVar9 != 8);
      }
    }
    iVar7 = iVar7 + 1;
    pcVar8 = pcVar8 + 0x1a;
    if (iVar7 == 0xc) {
      wch_restore_12();
      return;
    }
  } while( true );
}

