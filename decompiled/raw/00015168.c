/* Original address: 0x00015168; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

undefined4 FUN_ram_00015168(void)

{
  int iVar1;
  
  gp = &__global_pointer;
  display_fill(0);
  resource_draw_jpeg("menu/chinese.jpg",0x2a,0x70);
  resource_draw_jpeg("menu/english.jpg",0xba,0x70);
  FUN_ram_00008194(0x25,0x67,0xa9,0x8a,0xfd40);
  iVar1 = 1;
  do {
    if (button_event == '\x03') {
      button_event = '\0';
      if (iVar1 != 2) goto LAB_ram_0001525e;
LAB_ram_000151ec:
      FUN_ram_00008194(0x25,0x67,0xa9,0x8a,0xfd40);
      FUN_ram_00008194(0xb5,0x67,0x139,0x8a,0);
      iVar1 = 1;
    }
    else {
      if (button_event != '\x04') {
        if (iVar1 == 2) goto LAB_ram_0001525e;
        goto LAB_ram_000151ec;
      }
      button_event = '\0';
      if (iVar1 != 1) goto LAB_ram_000151ec;
LAB_ram_0001525e:
      FUN_ram_00008194(0xb5,0x67,0x139,0x8a,0xfd40);
      FUN_ram_00008194(0x25,0x67,0xa9,0x8a,0);
      iVar1 = 2;
    }
    if (button_event == '\x05') {
      if (iVar1 == 2) {
        resource_draw_jpeg("menu/english_.jpg",0xba,0x70);
      }
      else {
        resource_draw_jpeg("menu/chinese_.jpg",0x2a,0x70);
      }
      settings_language_access(1,iVar1 == 2);
      button_event = 0;
      return 0;
    }
  } while( true );
}

