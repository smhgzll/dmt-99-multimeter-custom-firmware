/* Original address: 0x0001a468; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0001a468(int param_1,uint param_2,short param_3,undefined2 param_4,undefined2 param_5,
                     undefined4 param_6)

{
  ushort uVar1;
  ushort uVar2;
  short sVar3;
  char cVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined2 uVar10;
  uint uVar11;
  short sVar12;
  ushort uVar13;
  char *pcVar14;
  uint uVar15;
  uint uVar16;
  undefined8 uVar17;
  
  gp = &__global_pointer;
  sVar12 = param_3 + 3;
  iVar6 = FUN_ram_0001da36(param_6,0x42c60000);
  if (iVar6 < 1) {
    iVar6 = FUN_ram_0001d9da(param_6,0);
    if (iVar6 == 0) {
      uVar7 = 0x434d0000;
    }
    else {
      FUN_ram_0001dddc(0x42c80000,param_6);
      uVar17 = FUN_ram_0001e2e2();
      FUN_ram_0001c3b8((int)uVar17,(int)((ulonglong)uVar17 >> 0x20),0xe147ae14,&DAT_ram_4000147a);
      uVar7 = FUN_ram_0001e38e();
    }
  }
  else {
    uVar7 = 0;
    sVar12 = 0;
  }
  uVar1 = *(ushort *)(param_1 + 2);
  uVar2 = *(ushort *)(param_1 + 4);
  uVar11 = uVar1 >> 3 & 0xff;
  pcVar14 = (char *)(param_1 + 6);
  for (uVar13 = 0; sVar3 = param_3 + uVar13, (uVar13 & 0xff) < uVar2; uVar13 = uVar13 + 1) {
    uVar16 = 0;
    uVar5 = param_2 & 0xffff;
    while( true ) {
      uVar5 = uVar5 + 8 & 0xffff;
      uVar15 = uVar5 - 8 & 0xffff;
      if (uVar11 <= (uVar16 & 0xff)) break;
      cVar4 = pcVar14[uVar16];
      do {
        if (cVar4 < '\0') {
          uVar8 = FUN_ram_0001e21c(sVar3);
          uVar9 = FUN_ram_0001e21c(sVar12);
          uVar9 = FUN_ram_0001d3be(uVar9,uVar7);
          iVar6 = FUN_ram_0001da36(uVar8,uVar9);
          uVar10 = param_4;
          if (iVar6 < 0) {
            uVar10 = param_5;
          }
          FUN_ram_00007f00(uVar15,sVar3,uVar10);
        }
        uVar15 = uVar15 + 1 & 0xffff;
        cVar4 = cVar4 << 1;
      } while (uVar5 != uVar15);
      uVar16 = uVar16 + 1;
    }
    pcVar14 = pcVar14 + uVar11;
    if ((uVar1 & 7) != 0) {
      cVar4 = *pcVar14;
      pcVar14 = pcVar14 + 1;
      uVar5 = 0;
      do {
        if (cVar4 < '\0') {
          uVar8 = FUN_ram_0001e21c(sVar3);
          uVar9 = FUN_ram_0001e21c(sVar12);
          uVar9 = FUN_ram_0001d3be(uVar9,uVar7);
          iVar6 = FUN_ram_0001da36(uVar8,uVar9);
          uVar10 = param_4;
          if (iVar6 < 0) {
            uVar10 = param_5;
          }
          FUN_ram_00007f00(((uVar1 >> 3 & 0xff) * 8 + (param_2 & 0xffff) & 0xffff) + uVar5 & 0xffff,
                           sVar3,uVar10);
        }
        uVar5 = uVar5 + 1;
        cVar4 = cVar4 << 1;
      } while ((uVar5 & 0xff) < (uVar1 & 7));
    }
  }
  wch_restore_12();
  return;
}

