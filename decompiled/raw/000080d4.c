/* Original address: 0x000080d4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000080d4(int param_1,uint param_2,int param_3,uint param_4,uint param_5,ushort param_6)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  for (uVar1 = param_2; uVar1 <= param_4; uVar1 = uVar1 + 1 & 0xffff) {
    FUN_ram_00007e38(param_1,uVar1);
    display_begin_pixels();
    for (uVar2 = 0; uVar2 < ((param_3 - param_1) + 1U & 0xffff); uVar2 = uVar2 + 1 & 0xffff) {
      display_write_data(param_6);
    }
  }
  for (uVar1 = 0; uVar1 < param_5; uVar1 = uVar1 + 1 & 0xff) {
    uVar2 = uVar1 + param_1 & 0xffff;
    uVar4 = ((param_5 - 1) + param_2) - uVar1 & 0xffff;
    FUN_ram_00008050(uVar2,param_2,uVar2,uVar4,0);
    uVar3 = uVar1 + (param_4 - param_5) + 1 & 0xffff;
    FUN_ram_00008050(uVar2,uVar3,uVar2,param_4,0);
    uVar2 = param_3 - uVar1 & 0xffff;
    FUN_ram_00008050(uVar2,param_2,uVar2,uVar4,0);
    FUN_ram_00008050(uVar2,uVar3,uVar2,param_4,0);
  }
  wch_restore_12();
  return;
}

