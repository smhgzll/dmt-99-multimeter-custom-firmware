/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001d728; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001d728; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0001d728(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar1 = param_1 >> 0x17 & 0xff;
  param_1 = param_1 & 0x7fffff;
  if (uVar1 == 0) {
    if (param_1 == 0) {
      uVar1 = 4;
    }
    else {
      FUN_ram_0001e4f8(param_1);
      uVar1 = 0;
    }
  }
  else if (uVar1 == 0xff) {
    if (param_1 == 0) {
      uVar1 = 8;
    }
    else {
      uVar1 = 0xc;
    }
  }
  else {
    uVar1 = 0;
  }
  uVar2 = param_2 >> 0x17 & 0xff;
  param_2 = param_2 & 0x7fffff;
  if (uVar2 == 0) {
    if (param_2 == 0) {
      uVar2 = 1;
LAB_ram_0001d7be:
                    /* WARNING: Could not recover jumptable at 0x0001d7d4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)(&UNK_ram_00027050 + *(int *)(&UNK_ram_00027050 + (uVar1 | uVar2) * 4)))();
      return;
    }
    FUN_ram_0001e4f8(param_2);
    uVar2 = 0;
  }
  else if (uVar2 == 0xff) {
    if (param_2 == 0) {
      uVar2 = 2;
      goto LAB_ram_0001d7be;
    }
    uVar2 = 3;
  }
  else {
    uVar2 = 0;
  }
                    /* WARNING: Could not recover jumptable at 0x0001d7b4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)((int)&DAT_ram_00027010 + (&DAT_ram_00027010)[uVar1 | uVar2]))();
  return;
}

