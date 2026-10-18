/* Original address: 0x00007e68; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00007e68(undefined4 param_1)

{
  ushort value;
  
  gp = &__global_pointer;
  switch(param_1) {
  case 0:
    value = 0xa0;
    break;
  case 1:
    value = 0xe0;
    break;
  case 2:
    value = 0x20;
    break;
  case 3:
    value = 0x60;
    break;
  case 4:
    value = 0x80;
    break;
  default:
    value = 0;
    break;
  case 6:
    value = 0xc0;
    break;
  case 7:
    value = 0x40;
  }
  display_write_register(0x36,value);
  display_write_command((ushort)(byte)DAT_ram_20000278);
  display_write_data(0);
  display_write_data(0);
  display_write_data((ushort)(display_width - 1 >> 8));
  display_write_data(display_width - 1 & 0xff);
  display_write_command((ushort)DAT_ram_20000278._1_1_);
  display_write_data(0);
  display_write_data(0);
  display_write_data((ushort)(display_height - 1 >> 8));
  display_write_data(display_height - 1 & 0xff);
  return;
}

