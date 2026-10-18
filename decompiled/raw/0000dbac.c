/* Original address: 0x0000dbac; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Extracts mode unit sign scale and magnitude from selected four-byte entry */

void history_decode_entry(int param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  iVar2 = (param_1 - 1U & 0xff) * 2;
  uVar1 = (uint)(ushort)(&DAT_ram_20000020)[iVar2 + 0x11];
  DAT_ram_20000228 = (byte)(&DAT_ram_20000020)[iVar2 + 0x11] & 0x3f;
  DAT_ram_20000229 = (byte)((int)uVar1 >> 6) & 0x3f;
  uVar3 = FUN_ram_0001e21c((&DAT_ram_20000020)[iVar2 + 0x12]);
  if (((int)uVar1 >> 0xd & 3U) == 3) {
    uVar4 = 0x447a0000;
  }
  else if ((int)(uVar1 << 0x11) < 0) {
    uVar4 = 0x42c80000;
  }
  else {
    if (-1 < (int)(uVar1 << 0x12)) goto LAB_ram_0000dc08;
    uVar4 = 0x41200000;
  }
  uVar3 = FUN_ram_0001d728(uVar3,uVar4);
LAB_ram_0000dc08:
  DAT_ram_2000022c = uVar3;
  if ((int)(uVar1 << 0x13) < 0) {
    DAT_ram_2000022c = DAT_ram_2000022c ^ 0x80000000;
  }
  return;
}

