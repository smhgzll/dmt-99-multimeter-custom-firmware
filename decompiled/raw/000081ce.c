/* Original address: 0x000081ce; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_000081ce(int param_1,int param_2,int param_3,int param_4,int param_5,undefined4 param_6
                     )

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  uVar3 = param_1 + param_5 & 0xffff;
  uVar2 = param_3 - param_5 & 0xffff;
  uVar1 = param_4 - param_5 & 0xffff;
  FUN_ram_00008050(uVar3,param_2,uVar2,param_2,param_6);
  uVar4 = param_5 + param_2 & 0xffff;
  FUN_ram_00008050(param_1,uVar4,param_1,uVar1,param_6);
  FUN_ram_00008050(uVar3,param_4,uVar2,param_4,param_6);
  FUN_ram_00008050(param_3,uVar4,param_3,uVar1,param_6);
  FUN_ram_00008050(uVar3,param_2,param_1,uVar4,param_6);
  FUN_ram_00008050(uVar2,param_2,param_3,uVar4,param_6);
  FUN_ram_00008050(param_1,uVar1,uVar3,param_4,param_6);
  FUN_ram_00008050(uVar2,param_4,param_3,uVar1,param_6);
  return;
}

