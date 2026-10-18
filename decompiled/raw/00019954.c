/* Original address: 0x00019954; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00019954(int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  gp = &__global_pointer;
  if (param_1 == 1) {
    FUN_ram_00008e46(0xbf,0x3a,&DAT_ram_00025380,0xffff,0xffff,0x18,1);
    FUN_ram_00008e46(0xbf,0x6c,&DAT_ram_0002538c,0xffff,0xffff,0x18,1);
    FUN_ram_00008e46(0xbf,0x9e,&DAT_ram_00025398,0xffff,0xffff,0x18,1);
    FUN_ram_00008194(0xa5,0x3c,0xb8,0x4f,0xffff);
    FUN_ram_00008194(0xa5,0x6e,0xb8,0x81,0xffff);
    FUN_ram_00008194(0xa5,0xa0,0xb8,0xb3,0xffff);
  }
  if (param_2 == 1) {
    FUN_ram_00008e46(0xbf,0x3a,&DAT_ram_00025380,0xfd40,0xfd40,0x18,1);
    uVar2 = 0x4f;
    uVar1 = 0x3c;
  }
  else if (param_2 == 2) {
    FUN_ram_00008e46(0xbf,0x6c,&DAT_ram_0002538c,0xfd40,0xfd40,0x18,1);
    uVar2 = 0x81;
    uVar1 = 0x6e;
  }
  else {
    if (param_2 != 3) goto LAB_ram_00019a44;
    FUN_ram_00008e46(0xbf,0x9e,&DAT_ram_00025398,0xfd40,0xfd40,0x18,1);
    uVar2 = 0xb3;
    uVar1 = 0xa0;
  }
  display_fill_rectangle(0xa5,uVar1,0xb8,uVar2,0xfd40);
LAB_ram_00019a44:
  if (param_3 == 1) {
    FUN_ram_00008e46(0xbf,0x3a,&DAT_ram_00025380,0xffff,0xffff,0x18,1);
    display_fill_rectangle(0xa5,0x3c,0xb8,0x4f,0);
    uVar2 = 0x4f;
    uVar1 = 0x3c;
  }
  else if (param_3 == 2) {
    FUN_ram_00008e46(0xbf,0x6c,&DAT_ram_0002538c,0xffff,0xffff,0x18,1);
    display_fill_rectangle(0xa5,0x6e,0xb8,0x81,0);
    uVar2 = 0x81;
    uVar1 = 0x6e;
  }
  else {
    if (param_3 != 3) {
      gp = &__global_pointer;
      return;
    }
    FUN_ram_00008e46(0xbf,0x9e,&DAT_ram_00025398,0xffff,0xffff,0x18,1);
    display_fill_rectangle(0xa5,0xa0,0xb8,0xb3,0);
    uVar2 = 0xb3;
    uVar1 = 0xa0;
  }
  FUN_ram_00008194(0xa5,uVar1,0xb8,uVar2,0xffff);
  return;
}

