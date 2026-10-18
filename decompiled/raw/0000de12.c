/* Original address: 0x0000de12; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Mounts resources removes LOGO.jpg then remounts */

void filesystem_logo_remove(void)

{
  gp = &__global_pointer;
  DAT_ram_20000230 = FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023cc0,1);
  if (DAT_ram_20000230 == 0) {
    FUN_ram_0000fab0("0:/LOGO.jpg");
  }
  DAT_ram_20000230 = FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023ccc,1);
  return;
}

