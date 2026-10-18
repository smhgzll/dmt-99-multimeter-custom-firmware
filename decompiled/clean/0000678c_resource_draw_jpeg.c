/* Semantic review: high. Opens path then invokes decoder with output callback and display coordinates.
 * Address 0x0000678c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000678c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: Opens path then invokes decoder with output callback and display coordinates */

undefined4 resource_draw_jpeg(undefined4 param_1,undefined2 param_2,undefined2 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *apuStack_10d0 [3];
  undefined1 auStack_10c4 [124];
  undefined1 auStack_1048 [4136];
  
  gp = &__global_pointer;
  iVar1 = FUN_ram_0000f54a(auStack_1048,param_1,1);
  if (iVar1 == 0) {
    apuStack_10d0[0] = auStack_1048;
    uVar2 = FUN_ram_0001e566(0x1c00);
    iVar1 = jpeg_decoder_prepare(auStack_10c4,jpeg_input_callback,uVar2,0x1c00,apuStack_10d0);
    if (iVar1 == 0) {
      jpeg_display_x = param_2;
      jpeg_display_y = param_3;
      jpeg_decoder_decompress(auStack_10c4,jpeg_display_callback,0);
    }
    FUN_ram_0001e570(uVar2);
    FUN_ram_0000f940(apuStack_10d0[0]);
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

