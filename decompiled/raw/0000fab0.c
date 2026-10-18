/* Original address: 0x0000fab0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

int FUN_ram_0000fab0(undefined4 param_1)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 auStack_284 [4];
  undefined1 *puStack_274;
  undefined1 *puStack_270;
  byte bStack_26a;
  undefined4 uStack_254;
  char cStack_245;
  undefined1 *apuStack_240 [2];
  int iStack_238;
  undefined1 auStack_210 [512];
  
  gp = &__global_pointer;
  auStack_284[0] = param_1;
  iVar2 = FUN_ram_0000e2b8(auStack_284,&puStack_274,2);
  if (iVar2 != 0) {
    return iVar2;
  }
  *(undefined1 **)(puStack_274 + 0x10) = auStack_210;
  puStack_270 = puStack_274;
  iVar2 = FUN_ram_0000ef70(&puStack_270,auStack_284[0]);
  puVar1 = puStack_274;
  if (iVar2 != 0) {
    return iVar2;
  }
  if (cStack_245 < '\0') {
    return 6;
  }
  if ((bStack_26a & 1) == 0) {
    iVar2 = FUN_ram_0000e0f8(*puStack_274,uStack_254);
    if ((bStack_26a & 0x10) != 0) {
      apuStack_240[0] = puVar1;
      iStack_238 = iVar2;
      iVar3 = FUN_ram_0000e7fc(apuStack_240,0);
      if (iVar3 != 0) {
        return iVar3;
      }
      iVar3 = FUN_ram_0000ec3e(apuStack_240,0);
      if (iVar3 == 0) goto LAB_ram_0000faec;
      if (iVar3 != 4) {
        return iVar3;
      }
    }
    iVar3 = FUN_ram_0000ed8e(&puStack_270);
    if ((iVar3 == 0) &&
       ((iVar2 == 0 || (iVar3 = FUN_ram_0000e9a4(&puStack_270,iVar2,0), iVar3 == 0)))) {
      iVar3 = FUN_ram_0000ea1e(puStack_274);
    }
  }
  else {
LAB_ram_0000faec:
    iVar3 = 7;
  }
  return iVar3;
}

