/* Semantic review: medium. Writes configured memory-write command.
 * Address 0x00007e20; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007e20; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: Writes configured memory-write command */

void display_begin_pixels(void)

{
  gp = &__global_pointer;
  display_write_command((ushort)DAT_ram_20000276._1_1_);
  return;
}

