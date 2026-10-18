/* Original address: 0x0000f940; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000f940(undefined4 *param_1)

{
  int iVar1;
  undefined1 auStack_14 [4];
  
  gp = &__global_pointer;
  iVar1 = FUN_ram_0000f8a4();
  if ((iVar1 == 0) && (iVar1 = FUN_ram_0000df10(param_1,auStack_14), iVar1 == 0)) {
    *param_1 = 0;
  }
  return;
}

