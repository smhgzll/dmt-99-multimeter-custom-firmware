/* Original address: 0x0001e6f4; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0001e6f4(undefined4 *param_1,uint param_2)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  uVar2 = (param_2 + 3 & 0xfffffffc) + 8;
  if (uVar2 < 0xc) {
    uVar2 = 0xc;
  }
  else if ((int)uVar2 < 0) goto LAB_ram_0001e750;
  if (param_2 <= uVar2) {
    FUN_ram_0001e81c();
    puVar3 = DAT_ram_20000268;
    puVar5 = DAT_ram_20000268;
    do {
      puVar1 = puVar3;
      if (puVar1 == (uint *)0x0) {
        if (DAT_ram_2000026c == 0) {
          DAT_ram_2000026c = FUN_ram_0001e7c2(param_1,0);
        }
        puVar3 = (uint *)FUN_ram_0001e7c2(param_1,uVar2);
        if ((puVar3 == (uint *)0xffffffff) ||
           ((puVar1 = (uint *)((int)puVar3 + 3U & 0xfffffffc), puVar3 != puVar1 &&
            (iVar4 = FUN_ram_0001e7c2(param_1,(int)puVar1 - (int)puVar3), iVar4 == -1)))) {
          *param_1 = 0xc;
          FUN_ram_0001e81e(param_1);
          return 0;
        }
LAB_ram_0001e776:
        *puVar1 = uVar2;
        puVar3 = DAT_ram_20000268;
LAB_ram_0001e784:
        DAT_ram_20000268 = puVar3;
        FUN_ram_0001e81e(param_1);
        uVar2 = (int)puVar1 + 0xbU & 0xfffffff8;
        iVar4 = uVar2 - (int)(puVar1 + 1);
        if (iVar4 != 0) {
          *(uint *)((int)puVar1 + iVar4) = (int)(puVar1 + 1) - uVar2;
          return uVar2;
        }
        return uVar2;
      }
      uVar6 = *puVar1 - uVar2;
      if (-1 < (int)uVar6) {
        if (uVar6 < 0xc) {
          puVar3 = (uint *)puVar1[1];
          if (puVar5 != puVar1) {
            puVar5[1] = puVar1[1];
            puVar3 = DAT_ram_20000268;
          }
          goto LAB_ram_0001e784;
        }
        *puVar1 = uVar6;
        puVar1 = (uint *)((int)puVar1 + uVar6);
        goto LAB_ram_0001e776;
      }
      puVar3 = (uint *)puVar1[1];
      puVar5 = puVar1;
    } while( true );
  }
LAB_ram_0001e750:
  *param_1 = 0xc;
  return 0;
}

