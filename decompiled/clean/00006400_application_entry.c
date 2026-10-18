/* Semantic review: high. Entry JAL to verified reset routine.
 * Address 0x00006400; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00006400; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: This function may have set the stack pointer */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* high: Entry JAL to verified reset routine */

void application_entry(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  gp = &__global_pointer;
  puVar1 = &DAT_ram_000271d0;
  puVar2 = &DAT_ram_20000000;
  do {
    *puVar2 = *puVar1;
    puVar1 = puVar1 + 1;
    puVar2 = puVar2 + 1;
  } while (puVar2 < &jpeg_display_x);
  puVar1 = (undefined4 *)&jpeg_display_x;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  } while (puVar1 < &DAT_ram_200054d8);
  _DAT_csreg_0bc0 = 0x1f;
  _DAT_csreg_0804 = 0xb;
  _mstatus = _mstatus | 0x6088;
  _mtvec = 0x643b;
  SystemInit();
  mepc = main;
  return;
}

