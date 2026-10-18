/* Original address: 0x00013504; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00013504(uint param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  char *pcVar3;
  undefined8 uVar4;
  
  gp = &__global_pointer;
  iVar2 = FUN_ram_0001dabc(param_1,0);
  uVar1 = param_1;
  if (iVar2 < 0) {
    uVar1 = param_1 ^ 0x80000000;
  }
  uVar4 = FUN_ram_0001e2e2(param_1);
  iVar2 = FUN_ram_0001dabc(uVar1,0x41200000);
  if (iVar2 < 0) {
    pcVar3 = "%0.3f%c";
  }
  else {
    iVar2 = FUN_ram_0001dabc(uVar1,0x42c80000);
    if (iVar2 < 0) {
      pcVar3 = "%0.2f%c";
    }
    else {
      iVar2 = FUN_ram_0001dabc(uVar1,0x447a0000);
      if (iVar2 < 0) {
        pcVar3 = "%0.1f%c";
      }
      else {
        pcVar3 = "%0.0f%c";
      }
    }
  }
  FUN_ram_0000969e(param_2,pcVar3,(int)uVar4,(int)((ulonglong)uVar4 >> 0x20),0);
  return;
}

