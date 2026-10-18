/* Original address: 0x00017c8c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00017c8c(undefined4 param_1,undefined4 param_2)

{
  ushort *puVar1;
  char cVar2;
  ushort *puVar3;
  
  gp = &__global_pointer;
  puVar1 = &DAT_ram_20003634;
  FUN_ram_000086f6(param_1,param_2,0xc4,0x14);
  display_begin_pixels();
  do {
    cVar2 = -0x3c;
    puVar3 = puVar1;
    do {
      cVar2 = cVar2 + -1;
      display_write_data(*puVar3);
      puVar3 = puVar3 + 0x14;
    } while (cVar2 != '\0');
    puVar1 = puVar1 + 1;
  } while (puVar1 != &DAT_ram_2000365c);
  return;
}

