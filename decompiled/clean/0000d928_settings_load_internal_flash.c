/* Semantic review: high. Copies 128 halfwords from 0x08040000 to 0x20000020.
 * Address 0x0000d928; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d928; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Copies 128 halfwords from 0x08040000 to 0x20000020 */

void settings_load_internal_flash(void)

{
  gp = &__global_pointer;
  read_halfwords((ushort *)0x8040000,&DAT_ram_20000020,0x80);
  return;
}

