/* Original address: 0x00010198; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00010198(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  gp = &__global_pointer;
  if (param_1 == 0) {
    uVar1 = 0x4228;
    uVar2 = uVar1;
    uVar3 = uVar1;
    uVar4 = uVar1;
    uVar5 = uVar1;
  }
  else if (param_1 == 1) {
    uVar1 = 0x4228;
    uVar2 = 0x4228;
    uVar3 = 0x4228;
    uVar4 = 0xfd40;
    uVar5 = 0x4228;
  }
  else if (param_1 == 2) {
    uVar1 = 0x4228;
    uVar2 = uVar1;
    uVar3 = uVar1;
    uVar4 = 0xfd40;
    uVar5 = 0xfd40;
  }
  else if (param_1 == 3) {
    uVar3 = 0xfd40;
    uVar1 = 0x4228;
    uVar2 = uVar1;
    uVar4 = uVar3;
    uVar5 = uVar3;
  }
  else if (param_1 == 4) {
    uVar2 = 0xfd40;
    uVar1 = 0x4228;
    uVar3 = uVar2;
    uVar4 = uVar2;
    uVar5 = uVar2;
  }
  else {
    uVar1 = 0;
    uVar2 = 0;
    uVar3 = 0;
    uVar4 = 0;
    uVar5 = 0;
    if (param_1 == 5) {
      uVar1 = 0xfd40;
      uVar2 = uVar1;
      uVar3 = uVar1;
      uVar4 = uVar1;
      uVar5 = uVar1;
    }
  }
  FUN_ram_00008050(0x15,0xd5,0x19,0xd5,uVar4);
  FUN_ram_00008050(0x19,0xd2,0x1c,0xd2,uVar4);
  FUN_ram_00007f00(0x1a,0xd4,uVar4);
  FUN_ram_00007f00(0x1b,0xd3,uVar4);
  FUN_ram_00008050(0x1c,0xcf,0x1f,0xcf,uVar5);
  FUN_ram_00007f00(0x1d,0xd1,uVar5);
  FUN_ram_00007f00(0x1e,0xd0,uVar5);
  FUN_ram_00008050(0x1f,0xcc,0x22,0xcc,uVar3);
  FUN_ram_00007f00(0x20,0xce,uVar3);
  FUN_ram_00007f00(0x21,0xcd,uVar3);
  FUN_ram_00008050(0x21,0xc9,0x24,0xc9,uVar2);
  FUN_ram_00007f00(0x23,0xcb,uVar2);
  FUN_ram_00007f00(0x24,0xca,uVar2);
  FUN_ram_00008050(0x24,0xc6,0x24,200,uVar1);
  FUN_ram_00008050(0x127,0xd5,299,0xd5,uVar4);
  FUN_ram_00008050(0x124,0xd2,0x127,0xd2,uVar4);
  FUN_ram_00007f00(0x126,0xd4,uVar4);
  FUN_ram_00007f00(0x125,0xd3,uVar4);
  FUN_ram_00008050(0x121,0xcf,0x124,0xcf,uVar5);
  FUN_ram_00007f00(0x123,0xd1,uVar5);
  FUN_ram_00007f00(0x122,0xd0,uVar5);
  FUN_ram_00008050(0x11e,0xcc,0x121,0xcc,uVar3);
  FUN_ram_00007f00(0x120,0xce,uVar3);
  FUN_ram_00007f00(0x11f,0xcd,uVar3);
  FUN_ram_00008050(0x11c,0xc9,0x11f,0xc9,uVar2);
  FUN_ram_00007f00(0x11d,0xcb,uVar2);
  FUN_ram_00007f00(0x11c,0xca,uVar2);
  FUN_ram_00008050(0x11c,0xc6,0x11c,200,uVar1);
  return;
}

