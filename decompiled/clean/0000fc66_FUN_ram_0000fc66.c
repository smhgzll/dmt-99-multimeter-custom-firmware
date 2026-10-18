/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000fc66; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000fc66; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000fc66(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  undefined4 uVar2;
  
  gp = &__global_pointer;
  uVar1 = disk_initialize();
  if ((uVar1 & 1) == 0) {
    uVar2 = 10;
    if ((uVar1 & 4) == 0) {
      uVar2 = 0x11;
      if (param_3 != 0) {
        uVar2 = FUN_ram_0000df50(param_1,param_2,7);
      }
    }
  }
  else {
    uVar2 = 3;
  }
  return uVar2;
}

