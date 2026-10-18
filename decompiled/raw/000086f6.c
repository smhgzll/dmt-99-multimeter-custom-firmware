/* Original address: 0x000086f6; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_000086f6(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  gp = &__global_pointer;
  iVar2 = param_3 + -1 + param_1;
  iVar1 = param_4 + -1 + param_2;
  display_write_command((ushort)(byte)DAT_ram_20000278);
  display_write_data((ushort)((uint)param_1 >> 8));
  display_write_data((ushort)param_1 & 0xff);
  display_write_data((ushort)((uint)iVar2 >> 8) & 0xff);
  display_write_data((ushort)((uint)(iVar2 * 0x10000) >> 0x10) & 0xff);
  display_write_command((ushort)DAT_ram_20000278._1_1_);
  display_write_data((ushort)((uint)param_2 >> 8));
  display_write_data((ushort)param_2 & 0xff);
  display_write_data((ushort)((uint)iVar1 >> 8) & 0xff);
  display_write_data((ushort)((uint)(iVar1 * 0x10000) >> 0x10) & 0xff);
  return;
}

