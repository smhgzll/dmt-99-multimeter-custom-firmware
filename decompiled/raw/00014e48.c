/* Original address: 0x00014e48; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00014e48(int param_1)

{
  byte bVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  undefined4 uVar5;
  byte bVar6;
  
  gp = &__global_pointer;
  bVar1 = 7;
  if (((((param_1 < 0x58) && (bVar1 = 6, param_1 < 0x4c)) && (bVar1 = 5, param_1 < 0x3f)) &&
      ((bVar1 = 4, param_1 < 0x33 && (bVar1 = 3, param_1 < 0x26)))) &&
     ((bVar1 = 2, param_1 < 0x1a && (bVar1 = 0, 0xc < param_1)))) {
    bVar1 = 1;
  }
  sVar2 = 0x8c;
  bVar6 = 1;
  do {
    if (bVar1 < bVar6) {
      uVar5 = 0x4228;
      if ((bVar6 != 1) || (bVar1 != 0)) goto LAB_ram_00014eb2;
      sVar4 = 0x90;
      sVar3 = 0x8c;
      uVar5 = 0xf800;
    }
    else {
      uVar5 = 0x7e0;
LAB_ram_00014eb2:
      sVar4 = sVar2 + 4;
      sVar3 = sVar2;
    }
    bVar6 = bVar6 + 1;
    sVar2 = sVar2 + 6;
    display_fill_rectangle(sVar3,6,sVar4,0x10,uVar5);
    if (bVar6 == 8) {
      return;
    }
  } while( true );
}

