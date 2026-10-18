/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000ddb0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ddb0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

bool FUN_ram_0000ddb0(void)

{
  char cVar1;
  bool bVar2;
  
  gp = &__global_pointer;
  DAT_ram_20000230 = FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023cc0,1);
  if (DAT_ram_20000230 == 0) {
    cVar1 = resource_draw_jpeg("0:/LOGO.jpg",0,0);
    FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023ccc,1);
    bVar2 = cVar1 != '\0';
  }
  else {
    FUN_ram_0000f4f8(&DAT_ram_200012cc,&DAT_ram_00023ccc,1);
    bVar2 = true;
  }
  return bVar2;
}

