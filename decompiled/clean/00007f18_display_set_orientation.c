/* Semantic review: high. 320x240 geometry and orientation command selection.
 * Address 0x00007f18; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007f18; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* high: 320x240 geometry and orientation command selection */

void display_set_orientation(int param_1)

{
  gp = &__global_pointer;
  if (param_1 == 0) {
    _display_width = 0x14000f0;
    DAT_ram_20000276 = 0x2c00;
    display_orientation = 5;
  }
  else {
    _display_width = 0xf00140;
    DAT_ram_20000276 = 0x2c01;
    display_orientation = 3;
  }
  DAT_ram_20000278 = 0x2b2a;
  FUN_ram_00007e68(display_orientation);
  return;
}

