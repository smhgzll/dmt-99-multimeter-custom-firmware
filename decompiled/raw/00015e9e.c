/* Original address: 0x00015e9e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00015e9e(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  uVar1 = settings_field_3_4_access(0,0);
  uVar2 = settings_temperature_access(0,0);
  if (display_layout == '\0') {
    display_fill_rectangle(0xa3,0xb,300,0x8d,0);
    display_fill_rectangle(0x8e,0x16,0x137,0xdb,0);
    FUN_ram_00014408(3);
    FUN_ram_000081ce(0x90,0x7c,0x126,0xd4,8,0x4228);
    FUN_ram_0001a360(&DAT_ram_00025488,0x9f,0x19,0,0xffff);
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_0001a360(&DAT_ram_00026f5c,0xa0,99,0,0xffff);
      FUN_ram_00008e46(0xc1,0x1b,&DAT_ram_000247c0,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0xb5,100,&DAT_ram_000247c8,0xffff,0xffff,0x10,1);
    }
    else {
      FUN_ram_0001a360(&DAT_ram_00026f5c,0x93,99,0,0xffff);
      FUN_ram_00008588(0xc1,0x1b,"Brightness",0xffff,0xffff,0x10,1);
      FUN_ram_00008588(0xa5,100,"Temperature unit",0xffff,0xffff,0x10,1);
    }
    if (uVar2 == 0) {
      display_fill_rectangle(0xa0,0x8c,0xb4,0xa0,0x1c9f);
      FUN_ram_00008194(0xa0,0xb1,0xb4,0xc5,0xffff);
      FUN_ram_00008e46(0xcf,0x8a,&DAT_ram_00025230,0x1c9f,0x1c9f,0x18,1);
      uVar4 = 0xffff;
    }
    else {
      FUN_ram_00008194(0xa0,0x8c,0xb4,0xa0,0xffff);
      display_fill_rectangle(0xa0,0xb1,0xb4,0xc5,0x1c9f);
      FUN_ram_00008e46(0xcf,0x8a,&DAT_ram_00025230,0xffff,0xffff,0x18,1);
      uVar4 = 0x1c9f;
    }
    FUN_ram_00008e46(0xcf,0xb0,&DAT_ram_00025234,uVar4,uVar4,0x18,1);
    FUN_ram_0001432e(uVar1);
  }
  else {
    display_fill_rectangle(0x83,0,0x140,0xf0,0);
    FUN_ram_00014408(3);
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008e46(0x8b,9,&DAT_ram_000247c0);
      FUN_ram_00008e46(0x8b,0x7d,&DAT_ram_000247c8,0xffff,0xffff,0x18,1);
      FUN_ram_000083d6(0xb8,9,0x3a,0x18,0xffff,0xffff,1);
      FUN_ram_000083d6(0xe7,0x7d,0x3a,0x18,0xffff,0xffff,1);
    }
    else {
      FUN_ram_00008588(0x8b,9,"Brightness:",0xffff,0xffff,0x18,1);
      FUN_ram_00008588(0x8b,0x7d,"Temperature unit:",0xffff,0xffff,0x10,1);
    }
    if (uVar2 == 0) {
      display_fill_rectangle(0x99,0xa1,0xac,0xb4,0xfd40);
      FUN_ram_00008194(0x99,0xcd,0xac,0xe0,0xffff);
      FUN_ram_00008e46(0xb4,0x9e,&DAT_ram_00025230,0xfd40,0xfd40,0x18,1);
      uVar4 = 0xffff;
    }
    else {
      display_fill_rectangle(0x99,0xcd,0xac,0xe0,0xfd40);
      FUN_ram_00008194(0x99,0xa1,0xac,0xb4,0xffff);
      FUN_ram_00008e46(0xb4,0x9e,&DAT_ram_00025230,0xffff,0xffff,0x18,1);
      uVar4 = 0xfd40;
    }
    FUN_ram_00008e46(0xb4,0xca,&DAT_ram_00025234,uVar4,uVar4,0x18,1);
    FUN_ram_00008050(0x87,0x71,0x137,0x71,0x4228);
    FUN_ram_000142a4(1,3,uVar1);
  }
  return;
}

