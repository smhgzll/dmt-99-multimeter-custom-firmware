/* Original address: 0x0001a124; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_0001a124(int param_1,uint param_2)

{
  int iVar1;
  undefined *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  
  gp = &__global_pointer;
  iVar1 = 0;
  if (param_1 == 0) {
    uVar4 = 0xffff;
    uVar3 = 0x4228;
    uVar6 = uVar4;
  }
  else {
    uVar3 = 0x1c9f;
    uVar4 = uVar3;
    uVar6 = 0;
  }
  if (param_2 == 1) {
    FUN_ram_00008266(0x119,0x2b,0x56,0x10);
  }
  else if (param_2 == 2) {
    FUN_ram_00008266(0x119,0x40,0x56,0x10,uVar4,1);
    iVar1 = 0x15;
  }
  else if (param_2 == 3) {
    FUN_ram_00008266(0x119,0x6c,0x41,0x10,uVar4,1);
    iVar1 = 0x41;
  }
  else if (param_2 == 4) {
    FUN_ram_00008266(0x119,0x82,0x41,0x10,uVar4,1);
    iVar1 = 0x57;
  }
  else if (param_2 == 5) {
    FUN_ram_00008266(0x117,0xac,0x5a,0x10,uVar4,1);
    iVar1 = 0x81;
  }
  else if (param_2 == 6) {
    FUN_ram_00008266(0x117,0xc3,0x5a,0x10,uVar4,1);
    iVar1 = 0x98;
  }
  iVar5 = iVar1 + 0x29;
  iVar7 = iVar1 + 0x3c;
  display_fill_rectangle(0xa6,iVar5,0xce,iVar7,uVar3);
  FUN_ram_00008050(0xa9,iVar5,0xa6,iVar1 + 0x2c,0);
  FUN_ram_00008050(0xa8,iVar5,0xa6,iVar1 + 0x2b,0);
  FUN_ram_00008050(0xa7,iVar5,0xa6,iVar1 + 0x2a,0);
  FUN_ram_00008050(0xa6,iVar5,0xa6,iVar5,0);
  FUN_ram_00008050(0xcf,iVar5,0x133,iVar5,uVar3);
  FUN_ram_00008050(0xcf,iVar7,0x130,iVar7,uVar3);
  FUN_ram_00008050(0x133,iVar1 + 0x2a,0x133,iVar1 + 0x39,uVar3);
  FUN_ram_00008050(0x130,iVar7,0x133,iVar1 + 0x39,uVar3);
  if (((param_2 & 0xfd) == 1) || (param_2 == 5)) {
    iVar5 = settings_language_access(0,0);
    if (iVar5 == 0) {
      puVar2 = &DAT_ram_00025460;
      goto LAB_ram_0001a300;
    }
    puVar2 = &DAT_ram_00025470;
  }
  else {
    iVar5 = settings_language_access(0,0);
    if (iVar5 == 0) {
      puVar2 = &DAT_ram_00025468;
LAB_ram_0001a300:
      FUN_ram_00008e46(0xa7,iVar1 + 0x2d,puVar2,uVar6,uVar6,0xc,1);
      gp = &__global_pointer;
      return;
    }
    puVar2 = &DAT_ram_00025474;
  }
  FUN_ram_00008588(0xae,iVar1 + 0x2b,puVar2,uVar6,uVar6,0x10);
  return;
}

