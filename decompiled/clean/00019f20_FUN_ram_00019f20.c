/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00019f20; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00019f20; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00019f20(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  undefined *puVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  
  gp = &__global_pointer;
  iVar1 = 0;
  if (param_1 == 0) {
    uVar4 = 0xffff;
    uVar5 = 0x4228;
    uVar6 = uVar4;
  }
  else {
    uVar4 = 0xfd40;
    uVar5 = uVar4;
    uVar6 = 0;
  }
  if (param_2 == 1) {
    FUN_ram_00008266(0x11e,0x1e,0x56,0x10);
  }
  else if (param_2 == 2) {
    FUN_ram_00008266(0x11e,0x39,0x56,0x10,uVar4,1);
    iVar1 = 0x1b;
  }
  else if (param_2 == 3) {
    FUN_ram_00008266(0x11e,0x6b,0x41,0x10,uVar4,1);
    iVar1 = 0x4d;
  }
  else if (param_2 == 4) {
    FUN_ram_00008266(0x11e,0x87,0x41,0x10,uVar4,1);
    iVar1 = 0x69;
  }
  else if (param_2 == 5) {
    FUN_ram_00008266(0x11c,0xb9,0x5a,0x10,uVar4,1);
    iVar1 = 0x9b;
  }
  else if (param_2 == 6) {
    FUN_ram_00008266(0x11c,0xd5,0x5a,0x10,uVar4,1);
    iVar1 = 0xb7;
  }
  iVar2 = iVar1 + 0x1b;
  iVar7 = iVar1 + 0x30;
  display_fill_rectangle(0x8c,iVar2,0xc0,iVar7,uVar5);
  FUN_ram_00008050(0x8c,iVar2,0x8c,iVar1 + 0x21,0);
  FUN_ram_00008050(0x8d,iVar2,0x8d,iVar1 + 0x20,0);
  FUN_ram_00008050(0x8e,iVar2,0x8e,iVar1 + 0x1f,0);
  FUN_ram_00008050(0x8f,iVar2,0x8f,iVar1 + 0x1e,0);
  FUN_ram_00008050(0x90,iVar2,0x90,iVar1 + 0x1d,0);
  FUN_ram_00008050(0x91,iVar2,0x91,iVar1 + 0x1c,0);
  FUN_ram_00007f00(0x92,iVar2,0);
  FUN_ram_00008050(0xc1,iVar2,0x135,iVar2,uVar5);
  FUN_ram_00008050(0x135,iVar2,0x135,iVar1 + 0x29,uVar5);
  FUN_ram_00008050(0x135,iVar1 + 0x29,0x12e,iVar7,uVar5);
  FUN_ram_00008050(0xc1,iVar7,0x12e,iVar7,uVar5);
  if (((param_2 & 0xfd) == 1) || (param_2 == 5)) {
    puVar3 = &DAT_ram_00025460;
  }
  else {
    puVar3 = &DAT_ram_00025468;
  }
  FUN_ram_00008e46(0x8f,iVar1 + 0x1f,puVar3,uVar6,uVar6,0x10,1);
  return;
}

