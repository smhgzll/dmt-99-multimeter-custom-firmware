/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00012c38; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00012c38; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00012c38(undefined4 param_1)

{
  undefined1 uVar1;
  
  gp = &__global_pointer;
  uVar1 = measurement_get_unit_code();
  FUN_ram_00012b00(uVar1,param_1,0xfd40);
  if (DAT_ram_2000024e != 0) {
    FUN_ram_00012b00((undefined1)DAT_ram_20000252,DAT_ram_2000024e,0xffff);
  }
  DAT_ram_20000252 = measurement_get_unit_code();
  DAT_ram_2000024e = (short)param_1;
  return;
}

