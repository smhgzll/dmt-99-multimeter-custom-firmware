/* Original address: 0x00012c78; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00012c78(undefined4 param_1)

{
  gp = &__global_pointer;
  display_fill_rectangle
            (0xf3,(uint)DAT_ram_200001dc * 0x16 + 0x23,299,(uint)DAT_ram_200001dc * 0x16 + 0x35,0);
  FUN_ram_000085cc(0xf3,(uint)DAT_ram_200001dc * 0x16 + 0x23,0x50,0x28,0x10,0xfd40,param_1);
  FUN_ram_00012c38((uint)DAT_ram_200001dc * 0x16 + 0x21);
  if (DAT_ram_200001dc != 1) {
    FUN_ram_000085cc(0xf3,(DAT_ram_200001dc - 1) * 0x16 + 0x23,0x50,0x28,0x10,0xffff,
                     &DAT_ram_20000240);
  }
  if (DAT_ram_20000250 == '\x01') {
    FUN_ram_000085cc(0xf3,0xd3,0x50,0x28,0x10,0xffff,&DAT_ram_20000240);
    DAT_ram_20000250 = '\0';
  }
  if (DAT_ram_200001dc < 8) {
    DAT_ram_200001dc = DAT_ram_200001dc + 1;
  }
  else {
    DAT_ram_200001dc = '\x01';
    DAT_ram_20000250 = '\x01';
  }
  FUN_ram_0001e80c(&DAT_ram_20000240,param_1);
  history_append_measurement();
  return;
}

