/* Original address: 0x00014c42; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00014c42(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined1 auStack_34 [20];
  
  gp = &__global_pointer;
  iVar2 = settings_language_access(0,0);
  FUN_ram_0000969e(&DAT_ram_20003304,&DAT_ram_00024730,&resource_language_prefix,"close.jpg");
  resource_draw_jpeg(&DAT_ram_20003304,0x29,0x40);
  uVar1 = 0x5f;
  if ((iVar2 != 0) && (iVar2 != 3)) {
    uVar1 = 0x7c;
  }
  iVar2 = 3;
  do {
    FUN_ram_0000969e(auStack_34,&DAT_ram_00024740,"time/",iVar2,".jpg");
    resource_draw_jpeg(auStack_34,0x86,uVar1);
    iVar2 = iVar2 + -1;
    delay_ms(1000);
  } while (iVar2 != 0);
                    /* WARNING: Subroutine does not return */
  power_down();
}

