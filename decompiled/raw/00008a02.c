/* Original address: 0x00008a02; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Window bounds followed by repeated RGB565 color writes */

void display_fill_rectangle(int param_1,uint param_2,int param_3,uint param_4,ushort param_5)

{
  uint uVar1;
  
  gp = &__global_pointer;
  FUN_ram_000086f6(0,0,0x140,0xf0);
  for (; param_2 <= param_4; param_2 = param_2 + 1 & 0xffff) {
    FUN_ram_00007e38(param_1,param_2);
    display_begin_pixels();
    for (uVar1 = 0; uVar1 < ((param_3 - param_1) + 1U & 0xffff); uVar1 = uVar1 + 1 & 0xffff) {
      display_write_data(param_5);
    }
  }
  return;
}

