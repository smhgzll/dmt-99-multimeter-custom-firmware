/* Original address: 0x00008672; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00008672(uint param_1,uint param_2,int param_3,int param_4,uint param_5,
                     undefined4 param_6,char *param_7)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  uVar2 = param_3 + param_1;
  uVar3 = param_4 + param_2;
  uVar4 = param_1 & 0xff;
  while( true ) {
    if (0x5e < (byte)(*param_7 - 0x20U)) {
      return;
    }
    if ((uVar2 & 0xffff) <= param_1) {
      param_2 = param_2 + param_5 & 0xffff;
      param_1 = uVar4;
    }
    if ((uVar3 & 0xffff) <= param_2) break;
    FUN_ram_00008266(param_1,param_2,*param_7,param_5,param_6,0);
    uVar1 = param_5 >> 2;
    if (*param_7 != '.') {
      uVar1 = param_5 >> 1;
    }
    param_1 = param_1 + uVar1 & 0xffff;
    param_7 = param_7 + 1;
  }
  return;
}

