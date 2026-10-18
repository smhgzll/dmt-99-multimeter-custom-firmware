/* Original address: 0x00014a92; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00014a92(void)

{
  bool bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  
  gp = &__global_pointer;
  uVar14 = (uint)history_count;
  bVar2 = 6;
  if ((((uVar14 < 0x1a) && (bVar2 = 5, uVar14 < 0x15)) && (bVar2 = 4, uVar14 < 0x10)) &&
     (bVar2 = 3, uVar14 < 0xb)) {
    bVar2 = (5 < uVar14) + 1;
  }
  uVar13 = uVar14 % 5;
  uVar16 = 5;
  if (bVar2 == 6) {
    uVar16 = uVar13;
  }
  uVar15 = 5;
  if (bVar2 == 5) {
    uVar15 = uVar13;
  }
  uVar9 = 5;
  if (bVar2 == 4) {
    uVar9 = uVar13;
  }
  uVar10 = 5;
  if (bVar2 == 3) {
    uVar10 = uVar13;
  }
  uVar11 = 5;
  if (bVar2 == 2) {
    uVar11 = uVar13;
  }
  uVar12 = 5;
  if (bVar2 == 1) {
    uVar12 = uVar13;
  }
  bVar3 = 1;
  bVar1 = false;
  do {
    if ((button_event == '\x04') || (button_event == '\x06')) {
      if (display_layout == '\0') {
        uVar8 = 0xe4;
        uVar7 = 0x13f;
        uVar6 = 0xb;
        uVar5 = 0x13d;
      }
      else {
        uVar8 = 0xea;
        uVar7 = 0x13d;
        uVar6 = 5;
        uVar5 = 0x13b;
      }
      display_fill_rectangle(uVar5,uVar6,uVar7,uVar8,0);
      wch_restore_12();
      return;
    }
    if (button_event == '\x01') {
      if (bVar3 < bVar2) {
        bVar3 = bVar3 + 1;
LAB_ram_00014b74:
        uVar8 = 0xd7;
        if (display_layout != '\0') {
          uVar8 = 0xf0;
        }
        display_fill_rectangle(0x84,0x33,0x13b,uVar8,0);
      }
LAB_ram_00014b98:
      button_event = '\0';
LAB_ram_00014b9c:
      if (uVar14 < 0x1e) {
        switch(bVar3) {
        case 2:
          FUN_ram_00014408(2);
          bVar4 = 2;
          uVar13 = uVar11;
          break;
        case 3:
          FUN_ram_00014408(3);
          bVar4 = 3;
          uVar13 = uVar10;
          break;
        case 4:
          FUN_ram_00014408(4);
          bVar4 = 4;
          uVar13 = uVar9;
          break;
        case 5:
          FUN_ram_00014408(5);
          bVar4 = 5;
          uVar13 = uVar15;
          break;
        case 6:
          FUN_ram_00014408(6);
          bVar4 = 6;
          uVar13 = uVar16;
          break;
        default:
          FUN_ram_00014408(1);
          FUN_ram_00014578(1,uVar12,0);
          bVar3 = 1;
          goto LAB_ram_00014bc8;
        }
      }
      else {
        uVar13 = 5;
        bVar4 = bVar3;
      }
      FUN_ram_00014578(bVar4,uVar13,0);
    }
    else {
      if (button_event == '\x02') {
        if (bVar3 != 1) {
          bVar3 = bVar3 - 1;
          goto LAB_ram_00014b74;
        }
        goto LAB_ram_00014b98;
      }
      if (!bVar1) goto LAB_ram_00014b9c;
    }
LAB_ram_00014bc8:
    bVar1 = true;
  } while( true );
}

