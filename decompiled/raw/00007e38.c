/* Original address: 0x00007e38; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007e38(undefined4 param_1,undefined4 param_2)

{
  gp = &__global_pointer;
  display_write_command((ushort)(byte)DAT_ram_20000278);
  display_write_data((ushort)((uint)param_1 >> 8));
  display_write_data((ushort)param_1 & 0xff);
  display_write_command((ushort)DAT_ram_20000278._1_1_);
  display_write_data((ushort)((uint)param_2 >> 8));
  display_write_data((ushort)param_2 & 0xff);
  return;
}

