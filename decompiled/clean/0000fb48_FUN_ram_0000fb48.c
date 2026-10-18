/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000fb48; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000fb48; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

int FUN_ram_0000fb48(byte *param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  byte *apbStack_84 [2];
  undefined4 uStack_7c;
  char acStack_78 [24];
  int aiStack_60 [2];
  undefined4 uStack_58;
  undefined1 *puStack_44;
  
  gp = &__global_pointer;
  apbStack_84[0] = param_1;
  iVar2 = FUN_ram_0000e2b8(apbStack_84,&uStack_7c,2);
  if (iVar2 != 0) {
    return iVar2;
  }
  memset_recovered(acStack_78,0x20,0xb);
  uVar1 = 0;
  while (0x1f < *apbStack_84[0]) {
    uVar3 = FUN_ram_0000e086(apbStack_84);
    if (0xffff < uVar3) {
      return 6;
    }
    uVar4 = FUN_ram_0000fd46();
    uVar3 = FUN_ram_0000fc9e(uVar4,0x3a8);
    if (uVar3 == 0) {
      return 6;
    }
    iVar2 = FUN_ram_0001e7f4(&DAT_ram_00023ce0,uVar3);
    if (iVar2 != 0) {
      return 6;
    }
    if (uVar3 < 0x100) {
      if (10 < uVar1) {
        return 6;
      }
    }
    else {
      if (9 < uVar1) {
        return 6;
      }
      acStack_78[uVar1] = (char)(uVar3 >> 8);
      uVar1 = uVar1 + 1;
    }
    acStack_78[uVar1] = (char)uVar3;
    uVar1 = uVar1 + 1;
  }
  if (acStack_78[0] == -0x1b) {
    return 6;
  }
  do {
    uVar3 = uVar1;
    if (uVar3 == 0) break;
    uVar1 = uVar3 - 1;
  } while (acStack_78[uVar3 - 1] == ' ');
  aiStack_60[0] = uStack_7c;
  uStack_58 = 0;
  iVar2 = FUN_ram_0000e7fc(aiStack_60,0);
  if (iVar2 != 0) {
    return iVar2;
  }
  iVar2 = FUN_ram_0000ec3e(aiStack_60,1);
  if (iVar2 == 0) {
    if (uVar3 == 0) {
      *puStack_44 = 0xe5;
      goto LAB_ram_0000fc0c;
    }
  }
  else {
    if (iVar2 != 4) {
      return iVar2;
    }
    if (uVar3 == 0) {
      return 0;
    }
    iVar2 = FUN_ram_0000ebf0(aiStack_60,1);
    if (iVar2 != 0) {
      return iVar2;
    }
    memset_recovered(puStack_44,0,0x20);
    puStack_44[0xb] = 8;
  }
  _memcpy(puStack_44,acStack_78,0xb);
LAB_ram_0000fc0c:
  *(undefined1 *)(uStack_7c + 3) = 1;
  iVar2 = FUN_ram_0000ea1e();
  return iVar2;
}

