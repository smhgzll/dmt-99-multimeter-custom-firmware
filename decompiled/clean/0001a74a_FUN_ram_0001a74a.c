/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001a74a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001a74a; Ghidra pseudocode, not compilable reconstructed source. */

undefined4 FUN_ram_0001a74a(int *param_1,undefined4 param_2)

{
  undefined4 auStack_14 [5];
  
  gp = &__global_pointer;
  if (param_1 == (int *)0x0) {
    auStack_14[0] = param_2;
    debug_putchar(0,auStack_14,1);
  }
  else {
    if (*param_1 != 0) {
      if (param_1[1] == 0) {
        gp = &__global_pointer;
        return 0xffffffff;
      }
      if (param_1[1] == 1) {
        *(undefined1 *)param_1[2] = 0;
        param_1[1] = 0;
        return 1;
      }
    }
    *(char *)param_1[2] = (char)param_2;
    param_1[2] = param_1[2] + 1;
    param_1[1] = param_1[1] + -1;
  }
  return 1;
}

