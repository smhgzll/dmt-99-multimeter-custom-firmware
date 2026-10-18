/* Semantic review: high. GPIOA pin 8 controls recharge image loop.
 * Address 0x00014cd6; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00014cd6; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* high: GPIOA pin 8 controls recharge image loop */

void charge_screen_poll(void)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = GPIO_ReadInputDataBit(&GPIOA,0x100);
  if (uVar1 != 0) {
    display_fill(0);
    resource_draw_jpeg("menu/recharge.jpg",0x7b,0x16);
    FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"recharge1.jpg");
    resource_draw_jpeg(&DAT_ram_20003304,0,0x57);
    FUN_ram_0000fdf6();
    do {
      uVar1 = GPIO_ReadInputDataBit(&GPIOA,0x100);
    } while (uVar1 != 0);
    DAT_ram_20000204 = 0;
    DAT_ram_20000203 = 0;
    DAT_ram_20000200 = 0;
    DAT_ram_20000224 = 1;
                    /* WARNING: Subroutine does not return */
    application_loop();
  }
  return;
}

