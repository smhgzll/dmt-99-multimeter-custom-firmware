/* Original address: 0x0001a336; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0001a336(void)

{
  char cVar1;
  
  gp = &__global_pointer;
  cVar1 = '\x01';
  do {
    if (display_layout == '\0') {
      FUN_ram_0001a124();
    }
    else {
      FUN_ram_00019f20(0,cVar1);
    }
    cVar1 = cVar1 + '\x01';
  } while (cVar1 != '\a');
  return;
}

