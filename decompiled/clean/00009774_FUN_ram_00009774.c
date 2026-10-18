/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00009774; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00009774; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00009774(char *param_1)

{
  char cVar1;
  int iVar2;
  char acStack_1c [12];
  
  gp = &__global_pointer;
  iVar2 = 0;
  while( true ) {
    cVar1 = *param_1;
    if (cVar1 == '\0') break;
    if (((byte)(cVar1 - 0x30U) < 10) || ((byte)(cVar1 - 0x2dU) < 2)) {
      acStack_1c[iVar2] = cVar1;
      iVar2 = iVar2 + 1;
    }
    param_1 = param_1 + 1;
  }
  acStack_1c[iVar2] = '\0';
  FUN_ram_000096de(acStack_1c);
  return;
}

