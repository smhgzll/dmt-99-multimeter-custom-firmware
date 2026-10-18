/* Original address: 0x00006734; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Pixel rectangle callback passed to decoder */

undefined4 jpeg_display_callback(undefined4 param_1,undefined2 *param_2,ushort *param_3)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  FUN_ram_000086f6((uint)jpeg_display_x + (uint)*param_3,(uint)jpeg_display_y + (uint)param_3[2],
                   ((uint)param_3[1] - (uint)*param_3) + 1,((uint)param_3[3] - (uint)param_3[2]) + 1
                  );
  display_begin_pixels();
  for (uVar2 = (uint)param_3[2]; uVar2 <= param_3[3]; uVar2 = uVar2 + 1) {
    uVar3 = 0;
    while( true ) {
      if (((uint)param_3[1] - (uint)*param_3) + 1 <= uVar3) break;
      uVar1 = *param_2;
      uVar3 = uVar3 + 1;
      param_2 = param_2 + 1;
      FUN_ram_00007e2e(uVar1);
    }
  }
  return 1;
}

