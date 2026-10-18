/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00019ba8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00019ba8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00019ba8(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  gp = &__global_pointer;
  if (param_1 == 1) {
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0x3c,&DAT_ram_00025380);
      FUN_ram_00008e46(0xbf,0x6e,&DAT_ram_0002538c,0xffff,0xffff,0x10,1);
      FUN_ram_00008e46(0xbf,0xa0,&DAT_ram_00025398,0xffff,0xffff,0x10,1);
    }
    else {
      FUN_ram_00008588(0xbf,0x3c,"Regular",0xffff,0xffff,0x10,1);
      FUN_ram_00008588(0xbf,0x6e,"Recording",0xffff,0xffff,0x10,1);
      FUN_ram_00008588(0xbf,0xa0,"Monitor",0xffff,0xffff,0x10,1);
    }
    FUN_ram_00008194(0xa5,0x3c,0xb6,0x4d,0xffff);
    FUN_ram_00008194(0xa5,0x6e,0xb6,0x7f,0xffff);
    FUN_ram_00008194(0xa5,0xa0,0xb6,0xb1,0xffff);
  }
  if (param_2 == 1) {
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0x3c,&DAT_ram_00025380);
    }
    else {
      FUN_ram_00008588(0xbf,0x3c,"Regular",0x1c9f,0x1c9f,0x10,1);
    }
    uVar3 = 0x4d;
    uVar2 = 0x3c;
  }
  else if (param_2 == 2) {
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0x6e,&DAT_ram_0002538c);
    }
    else {
      FUN_ram_00008588(0xbf,0x6e,"Recording",0x1c9f,0x1c9f,0x10,1);
    }
    uVar3 = 0x7f;
    uVar2 = 0x6e;
  }
  else {
    if (param_2 != 3) goto LAB_ram_00019cb4;
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0xa0,&DAT_ram_00025398);
    }
    else {
      FUN_ram_00008588(0xbf,0xa0,"Monitor",0x1c9f,0x1c9f,0x10,1);
    }
    uVar3 = 0xb1;
    uVar2 = 0xa0;
  }
  display_fill_rectangle(0xa5,uVar2,0xb6,uVar3,0x1c9f);
LAB_ram_00019cb4:
  if (param_3 == 1) {
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0x3c,&DAT_ram_00025380);
    }
    else {
      FUN_ram_00008588(0xbf,0x3c,"Regular",0xffff,0xffff,0x10,1);
    }
    display_fill_rectangle(0xa5,0x3c,0xb6,0x4d,0);
    uVar3 = 0x4d;
    uVar2 = 0x3c;
  }
  else if (param_3 == 2) {
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0x6e,&DAT_ram_0002538c);
    }
    else {
      FUN_ram_00008588(0xbf,0xa0,"Recording",0xffff,0xffff,0x10,1);
    }
    display_fill_rectangle(0xa5,0x6e,0xb6,0x7f,0);
    uVar3 = 0x7f;
    uVar2 = 0x6e;
  }
  else {
    if (param_3 != 3) {
      gp = &__global_pointer;
      return;
    }
    iVar1 = settings_language_access(0,0);
    if (iVar1 == 0) {
      FUN_ram_00008e46(0xbf,0xa0,&DAT_ram_00025398);
    }
    else {
      FUN_ram_00008588(0xbf,0xa0,"Monitor",0xffff,0xffff,0x10,1);
    }
    display_fill_rectangle(0xa5,0xa0,0xb6,0xb1,0);
    uVar3 = 0xb1;
    uVar2 = 0xa0;
  }
  FUN_ram_00008194(0xa5,uVar2,0xb6,uVar3,0xffff);
  return;
}

