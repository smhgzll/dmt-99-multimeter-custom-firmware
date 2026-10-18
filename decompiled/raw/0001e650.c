/* Original address: 0x0001e650; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0001e650(undefined4 *param_1,int param_2)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    return;
  }
  piVar1 = (int *)(param_2 - 4);
  if (*(int *)(param_2 + -4) < 0) {
    piVar1 = (int *)((int)piVar1 + *(int *)(param_2 + -4));
  }
  FUN_ram_0001e81c();
  if (DAT_ram_20000268 == (int *)0x0) {
    piVar1[1] = 0;
    DAT_ram_20000268 = piVar1;
  }
  else {
    piVar4 = DAT_ram_20000268;
    if (piVar1 < DAT_ram_20000268) {
      if (DAT_ram_20000268 == (int *)((int)piVar1 + *piVar1)) {
        iVar3 = *DAT_ram_20000268;
        DAT_ram_20000268 = (int *)DAT_ram_20000268[1];
        *piVar1 = iVar3 + *piVar1;
      }
      piVar1[1] = (int)DAT_ram_20000268;
      DAT_ram_20000268 = piVar1;
    }
    else {
      do {
        piVar6 = piVar4;
        piVar4 = (int *)piVar6[1];
        if (piVar4 == (int *)0x0) break;
      } while (piVar4 <= piVar1);
      piVar2 = (int *)((int)piVar6 + *piVar6);
      if (piVar2 == piVar1) {
        iVar3 = *piVar6 + *piVar1;
        *piVar6 = iVar3;
        if (piVar4 == (int *)((int)piVar6 + iVar3)) {
          iVar5 = piVar4[1];
          *piVar6 = iVar3 + *piVar4;
          piVar6[1] = iVar5;
        }
      }
      else if (piVar1 < piVar2) {
        *param_1 = 0xc;
      }
      else {
        if (piVar4 == (int *)((int)piVar1 + *piVar1)) {
          iVar3 = *piVar4;
          piVar4 = (int *)piVar4[1];
          *piVar1 = iVar3 + *piVar1;
        }
        piVar1[1] = (int)piVar4;
        piVar6[1] = (int)piVar1;
      }
    }
  }
  FUN_ram_0001e81e(param_1);
  return;
}

