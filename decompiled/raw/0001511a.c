/* Original address: 0x0001511a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Optional on.jpg splash then measurement screen setup */

void application_show_initial_screen(void)

{
  char cVar1;
  
  gp = &__global_pointer;
  if (DAT_ram_2000024d == '\0') {
    DAT_ram_2000024d = '\x01';
    cVar1 = FUN_ram_0000ddb0();
    if (cVar1 == '\x01') {
      resource_draw_jpeg("0:/on.jpg",0,0);
    }
    FUN_ram_00008016();
    delay_ms(1000);
  }
  display_fill(0);
  FUN_ram_00014ef2();
  normal_operation = 1;
  return;
}

