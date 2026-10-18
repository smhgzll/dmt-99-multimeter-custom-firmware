/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000085cc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000085cc; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000085cc(uint param_1,uint param_2,int param_3,int param_4,uint param_5,
                     undefined4 param_6,char *param_7)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  uVar2 = param_3 + param_1;
  uVar3 = param_4 + param_2;
  uVar4 = param_1 & 0xff;
  while( true ) {
    if (0x5e < (byte)(*param_7 - 0x20U)) break;
    if ((uVar2 & 0xffff) <= param_1) {
      param_2 = param_2 + param_5 & 0xffff;
      param_1 = uVar4;
    }
    if ((uVar3 & 0xffff) <= param_2) break;
    FUN_ram_00008266(param_1,param_2,*param_7,param_5,param_6,0);
    cVar1 = *param_7;
    if (cVar1 == '.') {
      param_1 = param_1 + (param_5 >> 2);
LAB_ram_0000866a:
      param_1 = param_1 & 0xffff;
    }
    else {
      param_1 = param_1 + (param_5 >> 1) & 0xffff;
      if ((param_5 == 0x10) && (((byte)(cVar1 + 0xa8U) < 3 || (cVar1 == 'J')))) {
        param_1 = param_1 + 6;
        goto LAB_ram_0000866a;
      }
    }
    param_7 = param_7 + 1;
  }
  wch_restore_12();
  return;
}

