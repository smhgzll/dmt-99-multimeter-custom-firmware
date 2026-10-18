/* Original address: 0x0001a360; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_0001a360(int param_1,uint param_2,short param_3,undefined2 param_4,undefined2 param_5)

{
  ushort uVar1;
  ushort uVar2;
  char cVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 uVar6;
  uint uVar7;
  uint uVar8;
  ushort uVar9;
  uint uVar10;
  uint uVar11;
  
  gp = &__global_pointer;
  uVar1 = *(ushort *)(param_1 + 2);
  uVar2 = *(ushort *)(param_1 + 4);
  uVar10 = uVar1 >> 3 & 0xff;
  pcVar4 = (char *)(param_1 + 6);
  for (uVar9 = 0; (uVar9 & 0xff) < uVar2; uVar9 = uVar9 + 1) {
    uVar8 = 0;
    uVar11 = param_2 & 0xffff;
    while( true ) {
      uVar11 = uVar11 + 8 & 0xffff;
      uVar7 = uVar11 - 8 & 0xffff;
      if (uVar10 <= (uVar8 & 0xff)) break;
      cVar3 = pcVar4[uVar8];
      do {
        uVar6 = param_5;
        if (cVar3 < '\0') {
          uVar6 = param_4;
        }
        FUN_ram_00007f00(uVar7,param_3 + uVar9,uVar6);
        uVar7 = uVar7 + 1 & 0xffff;
        cVar3 = cVar3 << 1;
      } while (uVar11 != uVar7);
      uVar8 = uVar8 + 1;
    }
    pcVar5 = pcVar4 + uVar10;
    pcVar4 = pcVar5;
    if ((uVar1 & 7) != 0) {
      pcVar4 = pcVar5 + 1;
      cVar3 = *pcVar5;
      uVar11 = 0;
      do {
        uVar6 = param_4;
        if (-1 < cVar3) {
          uVar6 = param_5;
        }
        FUN_ram_00007f00(((uVar1 >> 3 & 0xff) * 8 + (param_2 & 0xffff) & 0xffff) + uVar11 & 0xffff,
                         param_3 + uVar9,uVar6);
        uVar11 = uVar11 + 1;
        cVar3 = cVar3 << 1;
      } while ((uVar11 & 0xff) < (uVar1 & 7));
    }
  }
  wch_restore_12();
  return;
}

