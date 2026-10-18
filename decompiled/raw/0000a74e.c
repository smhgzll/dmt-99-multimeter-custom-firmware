/* Original address: 0x0000a74e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000a74e(void)

{
  char cVar1;
  int iVar2;
  uchar auStack_18 [8];
  
  gp = &__global_pointer;
  measurement_decode_segments(auStack_18);
  iVar2 = 0;
  cVar1 = '\0';
  do {
    if (auStack_18[iVar2] == '\x10') {
      cVar1 = cVar1 + '\x01';
    }
    iVar2 = iVar2 + 1;
  } while (iVar2 != 4);
  FUN_ram_0000fe08(cVar1);
  return;
}

