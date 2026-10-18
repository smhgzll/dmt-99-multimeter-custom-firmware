/* Original address: 0x00006708; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: File read/seek adapter passed to decoder */

int jpeg_input_callback(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iStack_14;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    iVar1 = FUN_ram_0000f962(**(int **)(param_1 + 0x78),
                             *(int *)(**(int **)(param_1 + 0x78) + 0x14) + param_3);
    if (iVar1 != 0) {
      param_3 = 0;
    }
  }
  else {
    iVar1 = FUN_ram_0000f732();
    param_3 = iStack_14;
    if (iVar1 != 0) {
      return iVar1;
    }
  }
  return param_3;
}

