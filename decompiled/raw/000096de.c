/* Original address: 0x000096de; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_000096de(char *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  bool bVar7;
  int iVar8;
  
  gp = &__global_pointer;
  if (*param_1 == '-') {
    param_1 = param_1 + 1;
    uVar6 = 0xffffffff;
  }
  else {
    uVar6 = 1;
    if (*param_1 == '+') {
      param_1 = param_1 + 1;
      uVar6 = 1;
    }
  }
  iVar2 = 1;
  bVar7 = false;
  iVar3 = 0;
  iVar1 = 0;
  for (; iVar8 = (int)*param_1, iVar8 != 0; param_1 = param_1 + 1) {
    if (iVar8 == 0x2e) {
      bVar7 = true;
    }
    else if (bVar7) {
      iVar2 = iVar2 * 10;
      iVar3 = iVar3 * 10 + iVar8 + -0x30;
    }
    else {
      iVar1 = iVar1 * 10 + iVar8 + -0x30;
    }
  }
  uVar4 = FUN_ram_0001e21c(iVar3);
  uVar5 = FUN_ram_0001e21c(iVar2);
  uVar4 = FUN_ram_0001d728(uVar4,uVar5);
  uVar5 = FUN_ram_0001e21c(iVar1);
  uVar4 = FUN_ram_0001d3be(uVar4,uVar5);
  uVar6 = FUN_ram_0001e21c(uVar6);
  FUN_ram_0001db48(uVar6,uVar4);
  return;
}

