/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00010404; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00010404; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00010404(int param_1)

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
    uVar4 = 0x4228;
    uVar5 = 0xfd40;
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
  FUN_ram_00008050(0x1e,0x25,0x24,0x25,uVar5);
  FUN_ram_00007f00(0x24,0x26,uVar5);
  FUN_ram_00008050(0x20,0x21,0x23,0x21,uVar5);
  FUN_ram_00008050(0x24,0x22,0x24,0x24,uVar5);
  FUN_ram_00008050(0x1e,0x1e,0x21,0x1e,uVar4);
  FUN_ram_00007f00(0x22,0x1f,uVar4);
  FUN_ram_00007f00(0x23,0x20,uVar4);
  FUN_ram_00008050(0x1c,0x1b,0x1e,0x1b,uVar3);
  FUN_ram_00007f00(0x1f,0x1c,uVar3);
  FUN_ram_00007f00(0x20,0x1d,uVar3);
  FUN_ram_00008050(0x19,0x18,0x1b,0x18,uVar2);
  FUN_ram_00007f00(0x1c,0x19,uVar2);
  FUN_ram_00007f00(0x1d,0x1a,uVar2);
  FUN_ram_00008050(0x14,0x15,0x18,0x15,uVar1);
  FUN_ram_00007f00(0x19,0x16,uVar1);
  FUN_ram_00007f00(0x1a,0x17,uVar1);
  FUN_ram_00008050(0x11c,0x26,0x121,0x26,uVar5);
  FUN_ram_00008050(0x11d,0x22,0x120,0x22,uVar5);
  FUN_ram_00008050(0x11c,0x23,0x11c,0x25,uVar5);
  FUN_ram_00008050(0x11f,0x1f,0x122,0x1f,uVar4);
  FUN_ram_00007f00(0x11e,0x20,uVar4);
  FUN_ram_00007f00(0x11d,0x21,uVar4);
  FUN_ram_00008050(0x122,0x1c,0x124,0x1c,uVar3);
  FUN_ram_00007f00(0x121,0x1d,uVar3);
  FUN_ram_00007f00(0x120,0x1e,uVar3);
  FUN_ram_00008050(0x125,0x19,0x127,0x19,uVar2);
  FUN_ram_00007f00(0x124,0x1a,uVar2);
  FUN_ram_00007f00(0x123,0x1b,uVar2);
  FUN_ram_00008050(0x128,0x16,0x12d,0x16,uVar1);
  FUN_ram_00007f00(0x127,0x17,uVar1);
  FUN_ram_00007f00(0x126,0x18,uVar1);
  return;
}

