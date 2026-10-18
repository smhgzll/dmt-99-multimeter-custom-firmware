/* Original address: 0x0000dc46; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: Copies defaults and clears remaining 128-halfword settings buffer */

void settings_restore_defaults(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 *puVar3;
  
  gp = &__global_pointer;
  uVar1 = 0;
  do {
    (&DAT_ram_20000020)[uVar1] = (&DAT_ram_20000120)[uVar1];
    puVar3 = &DAT_ram_20000020 + (uVar1 + 1 & 0xff);
    while( true ) {
      uVar2 = uVar1 + 1;
      uVar1 = uVar2 & 0xff;
      if ((int)(uVar2 * 0x1000000) >> 0x18 < 0) {
        filesystem_logo_remove();
        DAT_ram_20000224 = 1;
        return;
      }
      if (uVar1 < 0x11) break;
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
  } while( true );
}

