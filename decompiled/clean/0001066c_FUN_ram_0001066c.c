/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0001066c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001066c; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_0001066c(uint param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  uint uVar8;
  undefined4 uVar9;
  uint uVar10;
  
  gp = &__global_pointer;
  param_1 = param_1 >> 1;
  uVar2 = param_1 & 0xffff;
  if (param_1 < 5) {
    if (param_1 == 0) {
      FUN_ram_000084da(0x132,0xd0,0,1,0xc,0,0x4228);
      FUN_ram_000084da(0xb,0xd0,0,1,0xc,0,0x4228);
LAB_ram_00010768:
      FUN_ram_00010198(0);
      goto LAB_ram_000106c0;
    }
    FUN_ram_000084da(0x132,0xd0,0,1,0xc,0,0xfd40);
    FUN_ram_000084da(0xb,0xd0,0,1,0xc,0,0xfd40);
    if (param_1 == 2) {
      FUN_ram_00010198(2);
      uVar2 = 2;
    }
    else if (param_1 < 3) {
      if (param_1 != 1) goto LAB_ram_00010768;
      FUN_ram_00010198(1);
      uVar2 = 1;
    }
    else if (param_1 == 3) {
      FUN_ram_00010198(3);
      uVar2 = 3;
    }
    else {
      if (param_1 != 4) goto LAB_ram_00010768;
      FUN_ram_00010198(4);
      uVar2 = 4;
    }
LAB_ram_00010782:
    FUN_ram_00010404(0);
    FUN_ram_000084da(0x127,0x24,0x5a,2,0xc,0,0x4228);
    FUN_ram_000084da(299,0x18,100,3,0xc,0,0x4228);
    FUN_ram_000084da(0x10,0x24,0x5a,2,0xc,0,0x4228);
    uVar9 = 0x4228;
    uVar7 = 3;
    uVar4 = 100;
    uVar6 = 0x18;
    uVar5 = 2;
  }
  else {
    FUN_ram_00010198(5);
    FUN_ram_000084da(0x132,0xd0,0,1,0xc,0,0xfd40);
    FUN_ram_000084da(0xb,0xd0,0,1,0xc,0,0xfd40);
    uVar2 = uVar2 + 1;
    if (0x31 < uVar2) {
      FUN_ram_00010404(5);
      FUN_ram_000084da(0x127,0x24,0x5a,2,0xc,0,0xfd40);
      FUN_ram_000084da(299,0x18,100,3,0xc,0,0xfd40);
      FUN_ram_000084da(0x10,0x24,0x5a,2,0xc,0,0xfd40);
      FUN_ram_000084da(2,0x18,100,3,0xc,0,0xfd40);
      uVar2 = 0x2d;
      goto LAB_ram_00010870;
    }
LAB_ram_000106c0:
    if (uVar2 < 0x2d) goto LAB_ram_00010782;
    if (uVar2 == 0x2f) {
      uVar4 = 2;
LAB_ram_0001092c:
      FUN_ram_00010404(uVar4);
    }
    else {
      if (0x2f < uVar2) {
        if (uVar2 == 0x30) {
          uVar4 = 3;
        }
        else {
          uVar4 = 4;
          if (uVar2 != 0x31) goto LAB_ram_000106de;
        }
        goto LAB_ram_0001092c;
      }
      uVar4 = 1;
      if (uVar2 == 0x2e) goto LAB_ram_0001092c;
    }
LAB_ram_000106de:
    FUN_ram_000084da(0x127,0x24,0x5a,2,0xc,0,0xfd40);
    uVar9 = 0xfd40;
    uVar7 = 2;
    uVar4 = 0x5a;
    uVar6 = 0x24;
    uVar5 = 0x10;
  }
  FUN_ram_000084da(uVar5,uVar6,uVar4,uVar7,0xc,0,uVar9);
  if (0x2d < uVar2) {
    uVar2 = 0x2d;
  }
  uVar2 = uVar2 & 0xffff;
LAB_ram_00010870:
  uVar10 = 0;
  cVar1 = '\n';
  do {
    uVar8 = (uVar10 & 0xff) % 5;
    iVar3 = uVar10 * -4 + 0xc3;
    if ((int)uVar10 < (int)(uVar2 - 5)) {
      if (uVar8 == 0) {
        FUN_ram_0001a360(&DAT_ram_00025114,0x11c,iVar3,0,0xfd40);
        FUN_ram_0001a360(&DAT_ram_000250fc,0x1e,iVar3,0,0xfd40);
        uVar8 = uVar10 * -4 + 0xc0 & 0xffff;
        FUN_ram_000084da(0x126,uVar8,cVar1,2,0xc,0,0xfd40);
        uVar4 = 0xfd40;
        goto LAB_ram_000108f6;
      }
      FUN_ram_0001a360(&DAT_ram_00025120,0x11c,iVar3,0,0xfd40);
      uVar4 = 0xfd40;
LAB_ram_00010946:
      FUN_ram_0001a360(&DAT_ram_00025108,0x21,iVar3,0,uVar4);
    }
    else {
      if (uVar8 != 0) {
        FUN_ram_0001a360(&DAT_ram_00025120,0x11c,iVar3,0,0x4228);
        uVar4 = 0x4228;
        goto LAB_ram_00010946;
      }
      FUN_ram_0001a360(&DAT_ram_00025114,0x11c,iVar3,0,0x4228);
      FUN_ram_0001a360(&DAT_ram_000250fc,0x1e,iVar3,0,0x4228);
      uVar8 = uVar10 * -4 + 0xc0 & 0xffff;
      FUN_ram_000084da(0x126,uVar8,cVar1,2,0xc,0,0x4228);
      uVar4 = 0x4228;
LAB_ram_000108f6:
      FUN_ram_000084da(0x10,uVar8,cVar1,2,0xc,0,uVar4);
      cVar1 = cVar1 + '\n';
    }
    uVar10 = uVar10 + 1;
    if (uVar10 == 0x28) {
      return;
    }
  } while( true );
}

