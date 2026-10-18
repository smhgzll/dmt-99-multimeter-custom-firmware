/* Original address: 0x0001e7f4; Ghidra pseudocode, not compilable reconstructed source. */

char * FUN_ram_0001e7f4(char *param_1,char param_2)

{
  gp = &__global_pointer;
  while( true ) {
    if (*param_1 == '\0') {
      if (param_2 != '\0') {
        param_1 = (char *)0x0;
      }
      return param_1;
    }
    if (*param_1 == param_2) break;
    param_1 = param_1 + 1;
  }
  return param_1;
}

