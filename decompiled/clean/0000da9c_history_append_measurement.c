/* Semantic review: high. Shifts 29 four-byte entries caps count at 30 and packs measurement.
 * Address 0x0000da9c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000da9c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* high: Shifts 29 four-byte entries caps count at 30 and packs measurement */

uint history_append_measurement(void)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  ushort uVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  
  gp = &__global_pointer;
  puVar5 = &DAT_ram_20000020;
  do {
    puVar6 = puVar5 + -2;
    puVar5[0x4b] = puVar5[0x49];
    puVar5[0x4c] = puVar5[0x4a];
    puVar5 = puVar6;
  } while (puVar6 != (undefined2 *)0x1fffffac);
  history_records[0].metadata = (uint16_t)measurement_mode;
  sVar1 = measurement_get_unit_code();
  history_records[0].metadata = sVar1 << 6 | history_records[0].metadata;
  uVar2 = measurement_overload_kind();
  if (uVar2 == 1) {
    uVar2 = 0x477fff00;
  }
  else {
    uVar2 = measurement_overload_kind();
    if (uVar2 == 2) {
      uVar2 = 0x476eee00;
    }
    else {
      uVar2 = measurement_get_value_bits();
      iVar3 = FUN_ram_0001dabc(uVar2,0);
      if (iVar3 < 0) {
        history_records[0].metadata = history_records[0].metadata | 0x1000;
        uVar2 = uVar2 ^ 0x80000000;
      }
      iVar3 = FUN_ram_0001dabc(uVar2,0x41200000);
      if (iVar3 < 0) {
        uVar2 = FUN_ram_0001db48(uVar2,0x447a0000);
        uVar4 = 0x6000;
      }
      else {
        iVar3 = FUN_ram_0001dabc(uVar2,0x42c80000);
        if (iVar3 < 0) {
          uVar2 = FUN_ram_0001db48(uVar2,0x42c80000);
          uVar4 = 0x4000;
        }
        else {
          iVar3 = FUN_ram_0001dabc(uVar2,0x447a0000);
          if (-1 < iVar3) goto LAB_ram_0000db38;
          uVar2 = FUN_ram_0001db48(uVar2,0x41200000);
          uVar4 = 0x2000;
        }
      }
      history_records[0].metadata = history_records[0].metadata | uVar4;
    }
  }
LAB_ram_0000db38:
  history_records[0].magnitude = FUN_ram_0001e1c6(uVar2);
  if (_history_count < 0x1e) {
    _history_count = _history_count + 1;
  }
  return 0;
}

