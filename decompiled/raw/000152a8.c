/* Original address: 0x000152a8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_000152a8(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  gp = &__global_pointer;
  uVar1 = 0xffff;
  if (param_1 == 0) {
    uVar2 = 0x528a;
LAB_ram_000152c2:
    FUN_ram_000081ce(0xae,0x9d,0x10b,0xb6,3,0x4228);
    uVar5 = 0xffff;
    uVar4 = 0x528a;
    if (param_1 == 2) goto LAB_ram_0001530a;
  }
  else {
    if (param_1 != 1) {
      uVar1 = 0;
      uVar2 = 0x1c9f;
      goto LAB_ram_000152c2;
    }
    uVar2 = 0x528a;
    uVar5 = 0;
    uVar4 = 0x1c9f;
  }
  FUN_ram_000081ce(0xae,0xbf,0x10b,0xd8,3,0x4228);
LAB_ram_0001530a:
  FUN_ram_000080d4(0xae,0x9d,0x10b,0xb6,3,uVar2);
  FUN_ram_000080d4(0xae,0xbf,0x10b,0xd8,3,uVar4);
  iVar3 = settings_language_access(0,0);
  if (iVar3 == 0) {
    FUN_ram_00008e46(0xb8,0xa4,&DAT_ram_00024f48,uVar1,uVar1,0xc,1);
    FUN_ram_00008e46(0xd0,0xc6,&DAT_ram_00024f58,uVar5,uVar5,0xc,1);
  }
  else {
    FUN_ram_00008588(0xc9,0xa1,"Reset",uVar1,uVar1,0x10);
    FUN_ram_00008588(0xc5,0xc3,"Return",uVar5,uVar5,0x10,1);
  }
  return;
}

