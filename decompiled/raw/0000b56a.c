/* Original address: 0x0000b56a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000b56a(void)

{
  uchar auStack_1c [2];
  undefined2 uStack_1a;
  undefined1 uStack_13;
  
  gp = &__global_pointer;
  _memcpy(auStack_1c,&DAT_ram_0001e82c,10);
  uStack_1a = CONCAT11(0xb,(undefined1)uStack_1a);
  uStack_13 = 0x10;
  measurement_send_command(auStack_1c);
  delay_ms(200);
  uStack_1a = 0xa002;
  uStack_13 = 0xa2;
  measurement_send_command(auStack_1c);
  return;
}

