/* Original address: 0x00008d6c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00008d6c(uint param_1,uint param_2,char *param_3,undefined4 param_4,undefined4 param_5,
                     uint param_6,int param_7)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  
  gp = &__global_pointer;
  if ((*param_3 == -0x2a) && (param_3[1] == -0x30)) {
    uVar4 = (param_6 & 0xffff) - 1 & 0xffff;
    uVar6 = 0;
    FUN_ram_000086f6(param_1,param_2,param_1 + uVar4 & 0xffff,param_2 + uVar4 & 0xffff);
    uVar4 = param_1;
    for (uVar2 = 0; uVar2 < (param_6 & 0xffff) * ((uint)((param_6 & 7) != 0) + (param_6 >> 3));
        uVar2 = uVar2 + 1 & 0xff) {
      bVar1 = (&DAT_ram_00023582)[uVar2];
      uVar7 = 0;
      do {
        uVar5 = (int)(uint)bVar1 >> (uVar7 & 0x1f) & 1;
        if (param_7 == 0) {
          uVar3 = param_4;
          if (uVar5 == 0) {
            uVar3 = param_5;
          }
          display_write_data((ushort)uVar3);
          uVar6 = uVar6 + 1 & 0xff;
          if (uVar6 % param_6 == 0) {
            uVar6 = 0;
            break;
          }
        }
        else {
          if (uVar5 != 0) {
            FUN_ram_00007f00(uVar4,param_2,param_4);
          }
          uVar4 = uVar4 + 1 & 0xffff;
          if (uVar4 - param_1 == param_6) {
            param_2 = param_2 + 1 & 0xffff;
            uVar4 = param_1;
            break;
          }
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 != 8);
    }
  }
  wch_restore_12();
  return;
}

