/* Original address: 0x00007e0a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Command then data transport */

void display_write_register(ushort command,ushort value)

{
  gp = &__global_pointer;
  display_write_command(command);
  display_write_data(value);
  return;
}

