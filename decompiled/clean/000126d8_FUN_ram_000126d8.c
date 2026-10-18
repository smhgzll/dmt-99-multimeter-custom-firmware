/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000126d8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000126d8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000126d8(void)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 uVar8;
  
  gp = &__global_pointer;
  uVar6 = DAT_ram_20000032 & 0x7fff;
  uVar5 = DAT_ram_20000034 & 0x7fff;
  uVar4 = DAT_ram_20000036 & 0x7fff;
  uVar2 = DAT_ram_20000038 & 0x7fff;
  uVar1 = DAT_ram_2000003a & 0x7fff;
  uVar7 = DAT_ram_2000003c & 0x7fff;
  uVar8 = 0x2b;
  if (display_layout != '\0') {
    uVar8 = 0x1e;
  }
  FUN_ram_00008266(0x101,uVar8,uVar6 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar8,(uVar6 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar6 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_20000032 < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  uVar8 = 0x39;
  if (display_layout == '\0') {
    uVar8 = 0x40;
  }
  FUN_ram_00008266(0x101,uVar8,uVar5 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar8,(uVar5 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar5 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_20000034 < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  uVar8 = 0x6a;
  if (display_layout == '\0') {
    uVar8 = 0x6c;
  }
  FUN_ram_00008266(0x107,uVar8,uVar4 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf8,uVar8,(uVar4 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xef,uVar8,0x2e,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar4 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_20000036 < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  uVar8 = 0x86;
  if (display_layout == '\0') {
    uVar8 = 0x82;
  }
  FUN_ram_00008266(0x107,uVar8,uVar2 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf8,uVar8,(uVar2 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xef,uVar8,0x2e,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar2 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_20000038 < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  uVar8 = 0xb8;
  if (display_layout == '\0') {
    uVar8 = 0xac;
  }
  FUN_ram_00008266(0x101,uVar8,uVar1 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar8,(uVar1 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar1 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_2000003a < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  uVar8 = 0xd5;
  if (display_layout == '\0') {
    uVar8 = 0xc3;
  }
  FUN_ram_00008266(0x101,uVar8,uVar7 % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xf2,uVar8,(uVar7 / 10) % 10 + 0x30,0x10,0xffff,0);
  FUN_ram_00008266(0xe3,uVar8,(uVar7 / 100) % 10 + 0x30,0x10,0xffff,0);
  if ((short)DAT_ram_2000003c < 0) {
    uVar3 = 0x2d;
  }
  else {
    uVar3 = 0x2b;
  }
  FUN_ram_000083d6(0xd4,uVar8,uVar3,0x10,0xffff,0);
  wch_restore_12();
  return;
}

