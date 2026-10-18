/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000099cc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000099cc; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_000099cc(uint param_1,undefined4 param_2)

{
  int iVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  
  gp = &__global_pointer;
  if ((measurement_mode == '\x12') ||
     (((measurement_mode == '\x16' || (measurement_mode == '\x11')) &&
      (measurement_unit_code == '\b')))) {
    iVar1 = FUN_ram_0001da36(param_1,0x447a0000);
    if (-1 < iVar1) goto LAB_ram_00009a0e;
    iVar1 = FUN_ram_0001da36(param_1,0x42c80000);
    if (-1 < iVar1) {
      uVar4 = FUN_ram_0001e2e2(param_1);
      uVar3 = 0x20;
LAB_ram_00009a54:
      FUN_ram_0000969e(param_2,&DAT_ram_0002393c,uVar3,0x30,(int)uVar4,
                       (int)((ulonglong)uVar4 >> 0x20),0);
      gp = &__global_pointer;
      return;
    }
    iVar1 = FUN_ram_0001da36(param_1,0x41200000);
    if (-1 < iVar1) {
      FUN_ram_0001e2e2(param_1);
      uVar3 = 0x20;
LAB_ram_00009a92:
      FUN_ram_0000969e(param_2,&DAT_ram_00023928,uVar3,0x30,0x30);
      gp = &__global_pointer;
      return;
    }
    iVar1 = FUN_ram_0001da36(param_1,0);
    if (-1 < iVar1) {
      uVar4 = FUN_ram_0001e2e2(param_1);
      uVar3 = 0x20;
LAB_ram_00009acc:
      FUN_ram_0000969e(param_2,&DAT_ram_00023938,uVar3,0x30,0x30,0x30,(int)uVar4,
                       (int)((ulonglong)uVar4 >> 0x20),0);
      gp = &__global_pointer;
      return;
    }
    iVar1 = FUN_ram_0001dabc(param_1,0xc47a0000);
    if (0 < iVar1) {
      iVar1 = FUN_ram_0001dabc(param_1,0xc2c80000);
      if (iVar1 < 1) {
        uVar4 = FUN_ram_0001e2e2(param_1 ^ 0x80000000);
        uVar3 = 0x2d;
        goto LAB_ram_00009a54;
      }
      iVar1 = FUN_ram_0001dabc(param_1,0xc1200000);
      if (iVar1 < 1) {
        FUN_ram_0001e2e2(param_1 ^ 0x80000000);
        uVar3 = 0x2d;
        goto LAB_ram_00009a92;
      }
      iVar1 = FUN_ram_0001da36(param_1,0xc1200000);
      if (iVar1 < 0) {
        gp = &__global_pointer;
        return;
      }
      uVar4 = FUN_ram_0001e2e2(param_1 ^ 0x80000000);
      uVar3 = 0x2d;
      goto LAB_ram_00009acc;
    }
  }
  else {
    iVar1 = FUN_ram_0001da36(param_1,0x447a0000);
    if (-1 < iVar1) {
LAB_ram_00009a0e:
      FUN_ram_0001e2e2(param_1);
      pcVar2 = "%c%0.0f%c";
LAB_ram_00009a26:
      FUN_ram_0000969e(param_2,pcVar2,0x20);
      return;
    }
    iVar1 = FUN_ram_0001da36(param_1,0x42c80000);
    if (-1 < iVar1) {
      FUN_ram_0001e2e2(param_1);
      pcVar2 = "%c%0.1f%c";
      goto LAB_ram_00009a26;
    }
    iVar1 = FUN_ram_0001da36(param_1,0x41200000);
    if (-1 < iVar1) {
      FUN_ram_0001e2e2(param_1);
      pcVar2 = "%c%0.2f%c";
      goto LAB_ram_00009a26;
    }
    iVar1 = FUN_ram_0001da36(param_1,0);
    if (-1 < iVar1) {
      FUN_ram_0001e2e2(param_1);
      pcVar2 = "%c%0.3f%c";
      goto LAB_ram_00009a26;
    }
    iVar1 = FUN_ram_0001dabc(param_1,0xc47a0000);
    if (0 < iVar1) {
      iVar1 = FUN_ram_0001dabc(param_1,0xc2c80000);
      if (iVar1 < 1) {
        uVar4 = FUN_ram_0001e2e2(param_1);
        pcVar2 = "%0.1f%c";
      }
      else {
        iVar1 = FUN_ram_0001dabc(param_1,0xc1200000);
        if (iVar1 < 1) {
          uVar4 = FUN_ram_0001e2e2(param_1);
          pcVar2 = "%0.2f%c";
        }
        else {
          iVar1 = FUN_ram_0001da36(param_1,0xc1200000);
          if (iVar1 < 0) {
            gp = &__global_pointer;
            return;
          }
          uVar4 = FUN_ram_0001e2e2(param_1);
          pcVar2 = "%0.3f%c";
        }
      }
      goto LAB_ram_00009b02;
    }
  }
  uVar4 = FUN_ram_0001e2e2(param_1);
  pcVar2 = "%0.0f%c";
LAB_ram_00009b02:
  FUN_ram_0000969e(param_2,pcVar2,(int)uVar4,(int)((ulonglong)uVar4 >> 0x20),0);
  gp = &__global_pointer;
  return;
}

