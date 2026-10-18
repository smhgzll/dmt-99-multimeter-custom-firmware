/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000f4f8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000f4f8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000f4f8(undefined1 *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uStack_28;
  undefined1 *apuStack_24 [4];
  undefined4 uStack_14;
  
  gp = &__global_pointer;
  uStack_28 = param_2;
  apuStack_24[0] = param_1;
  uStack_14 = param_2;
  iVar1 = FUN_ram_0000dec0(&uStack_14);
  if (iVar1 < 0) {
    uVar2 = 0xb;
  }
  else {
    if (*(undefined1 **)(&DAT_ram_20000234 + iVar1 * 4) != (undefined1 *)0x0) {
      **(undefined1 **)(&DAT_ram_20000234 + iVar1 * 4) = 0;
    }
    if (apuStack_24[0] != (undefined1 *)0x0) {
      *apuStack_24[0] = 0;
    }
    *(undefined1 **)(&DAT_ram_20000234 + iVar1 * 4) = apuStack_24[0];
    uVar2 = 0;
    if (param_3 != 0) {
      uVar2 = FUN_ram_0000e2b8(&uStack_28,apuStack_24,0);
    }
  }
  return uVar2;
}

