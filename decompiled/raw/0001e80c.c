/* Original address: 0x0001e80c; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0001e80c(char *param_1,char *param_2)

{
  char cVar1;
  
  gp = &__global_pointer;
  do {
    cVar1 = *param_2;
    param_2 = param_2 + 1;
    *param_1 = cVar1;
    param_1 = param_1 + 1;
  } while (cVar1 != '\0');
  return;
}

