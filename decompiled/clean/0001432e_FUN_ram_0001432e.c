/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001432e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001432e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0001432e(uint param_1)

{
  uint uVar1;
  short value;
  
  gp = &__global_pointer;
  uVar1 = 0;
  if (param_1 < 3) {
    uVar1 = param_1;
  }
  FUN_ram_000080d4(0x9a,0x36,0x11c,0x46,2,0);
  if (uVar1 == 0) {
    FUN_ram_000080d4(0x9a,0x36,0x11c,0x46,2,0x1c9f);
    value = 0;
  }
  else if (uVar1 == 1) {
    FUN_ram_000080d4(0x99,0x35,0xf1,0x47,2,0x1c9f);
    value = 1;
  }
  else {
    FUN_ram_000080d4(0x99,0x35,0xc5,0x47,2,0x1c9f);
    value = 2;
  }
  settings_field_3_4_access(1,value);
  FUN_ram_000081ce(0x99,0x35,0x11d,0x47,2,0x4228);
  return;
}

