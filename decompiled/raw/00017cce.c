/* Original address: 0x00017cce; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */
/* WARNING: Restarted to delay deadcode elimination for space: ram */

void FUN_ram_00017cce(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  uint uVar5;
  byte bVar6;
  byte bVar7;
  uint uVar8;
  int iVar9;
  undefined4 unaff_s5;
  undefined4 uVar10;
  uint uVar11;
  uint uVar12;
  undefined8 uVar13;
  
  gp = &__global_pointer;
  if (param_2 == 1) {
    memset_recovered(&DAT_ram_20003324,0,0x310);
    DAT_ram_20000260 = 0;
    DAT_ram_20000256 = 0;
    display_fill_rectangle(0x25,0x35,0xef,0xd8,0);
  }
  uVar8 = (uint)DAT_ram_20000256;
  iVar9 = (uVar8 - 1) * 4;
  if ((((measurement_mode == '\v') || (measurement_mode == '\n')) || (measurement_mode == '\x11'))
     || ((measurement_mode == '\x16' || (measurement_mode == '\x0f')))) {
    uVar5 = (uint)measurement_unit_code;
    if (((uVar5 - 8 & 0xff) < 0xb) && ((0x489U >> (uVar5 - 8 & 0x1f) & 1) != 0)) {
      uVar4 = FUN_ram_0001d728(measurement_value_bits,0x447a0000);
    }
    else {
      if (((uVar5 - 0xd & 0xfb) != 0) && (uVar5 != 0x14)) goto LAB_ram_00017e4e;
      uVar4 = FUN_ram_0001db48(measurement_value_bits,0x447a0000);
    }
    *(undefined4 *)(&DAT_ram_20003324 + iVar9) = uVar4;
  }
  else {
LAB_ram_00017e4e:
    *(undefined4 *)(&DAT_ram_20003324 + iVar9) = measurement_value_bits;
  }
  uVar11 = (uint)DAT_ram_20000260;
  uVar4 = 0x461c3c00;
  uVar5 = 0;
  do {
    if ((uVar5 & 0xffff) < uVar11) {
      uVar10 = *(undefined4 *)(&DAT_ram_20003324 + uVar5 * 4);
      iVar9 = FUN_ram_0001da36(uVar10,unaff_s5);
      if (0 < iVar9) {
        unaff_s5 = uVar10;
      }
      iVar9 = FUN_ram_0001dabc(uVar10,uVar4);
      if (iVar9 < 0) {
        uVar4 = uVar10;
      }
    }
    uVar5 = uVar5 + 1;
  } while (uVar5 != 0xc4);
  FUN_ram_0001dddc(unaff_s5,uVar4);
  uVar13 = FUN_ram_0001e2e2();
  uVar13 = FUN_ram_0001bd4e(0,0x40640000,(int)uVar13,(int)((ulonglong)uVar13 >> 0x20));
  if (uVar11 != 0) {
    DAT_ram_20000254 = 0;
    do {
      DAT_ram_2000025c = (undefined4)((ulonglong)uVar13 >> 0x20);
      DAT_ram_20000258 = (undefined4)uVar13;
      memset_recovered(&DAT_ram_20003634,0,0x1ea0);
      uVar5 = 0;
      uVar11 = uVar8;
      do {
        uVar1 = DAT_ram_2000025c;
        uVar10 = DAT_ram_20000258;
        uVar12 = uVar5 + 1;
        if (0xc1 < (int)(uVar5 + DAT_ram_20000260)) {
          FUN_ram_0001dddc(*(undefined4 *)(&DAT_ram_20003324 + ((int)uVar11 % 0xc4) * 4),uVar4);
          uVar13 = FUN_ram_0001e2e2();
          uVar13 = FUN_ram_0001c3b8((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),uVar10,uVar1);
          FUN_ram_0001c870(0,0x4063e000,(int)uVar13,(int)((ulonglong)uVar13 >> 0x20));
          uVar2 = FUN_ram_0001d016();
          FUN_ram_0001dddc(*(undefined4 *)(&DAT_ram_20003324 + ((int)(uVar11 + 1) % 0xc4) * 4),uVar4
                          );
          uVar13 = FUN_ram_0001e2e2();
          uVar13 = FUN_ram_0001c3b8((int)uVar13,(int)((ulonglong)uVar13 >> 0x20),uVar10,uVar1);
          FUN_ram_0001c870(0,0x4063e000,(int)uVar13,(int)((ulonglong)uVar13 >> 0x20));
          uVar3 = FUN_ram_0001d016();
          if (0x9f < uVar3) {
            uVar3 = 0x9f;
          }
          if (0x9f < uVar2) {
            uVar2 = 0x9f;
          }
          FUN_ram_00017c06(uVar5 & 0xffff,uVar2,uVar12 & 0xffff,uVar3,0xffff);
        }
        uVar5 = uVar12;
        uVar11 = uVar11 + 1;
      } while (uVar12 != 0xc3);
      FUN_ram_00017c8c(0x28,(uint)DAT_ram_20000254 * 0x14 + 0x37);
      uVar13 = CONCAT44(DAT_ram_2000025c,DAT_ram_20000258);
      bVar6 = DAT_ram_20000254 + 1;
      bVar7 = DAT_ram_20000254 + 1;
      DAT_ram_20000254 = bVar6;
    } while (bVar7 < 8);
  }
  DAT_ram_2000025c = (undefined4)((ulonglong)uVar13 >> 0x20);
  DAT_ram_20000258 = (undefined4)uVar13;
  DAT_ram_20000256 = (ushort)((DAT_ram_20000256 + 1) % 0xc5);
  DAT_ram_20000260 = DAT_ram_20000260 + 1;
  wch_restore_12();
  return;
}

