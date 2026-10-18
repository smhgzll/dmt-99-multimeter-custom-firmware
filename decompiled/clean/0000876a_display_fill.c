/* Semantic review: high. Width times height repeated color writes after window setup.
 * Address 0x0000876a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000876a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Width times height repeated color writes after window setup */

void display_fill(ushort param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  uVar3 = (uint)display_width;
  uVar1 = (uint)display_height;
  FUN_ram_000086f6(0,0,0x140,0xf0);
  FUN_ram_00007e38(0,0);
  display_begin_pixels();
  for (iVar2 = 0; iVar2 != uVar1 * uVar3; iVar2 = iVar2 + 1) {
    display_write_data(param_1);
  }
  return;
}

