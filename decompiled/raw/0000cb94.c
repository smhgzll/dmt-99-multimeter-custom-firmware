/* Original address: 0x0000cb94; Ghidra pseudocode, not compilable reconstructed source. */

/* high: PFIC enable disable and priority configuration */

void NVIC_Init(byte *param_1)

{
  byte bVar1;
  uint uVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  
  gp = &__global_pointer;
  bVar1 = *param_1;
  if (DAT_ram_2000020c == 0) {
    uVar4 = (uint)param_1[2];
LAB_ram_0000cba0:
    bVar3 = (byte)(uVar4 << 4);
  }
  else {
    if (DAT_ram_2000020c != 1) {
      if (DAT_ram_2000020c == 2) {
        uVar2 = (uint)param_1[1];
        uVar4 = (uint)param_1[2];
        if (1 < uVar2) {
          iVar5 = (uVar2 + 0x3e) * 4;
LAB_ram_0000cc02:
          bVar3 = (byte)((uVar4 + iVar5 & 0xff) << 4);
          goto LAB_ram_0000cbb6;
        }
        iVar5 = uVar2 << 2;
      }
      else {
        if (DAT_ram_2000020c != 3) {
          if (DAT_ram_2000020c != 4) goto LAB_ram_0000cbc8;
          uVar4 = (uint)param_1[1];
          goto LAB_ram_0000cba0;
        }
        uVar2 = (uint)param_1[1];
        uVar4 = (uint)param_1[2];
        if (3 < uVar2) {
          iVar5 = (uVar2 + 0x7c) * 2;
          goto LAB_ram_0000cc02;
        }
        iVar5 = uVar2 << 1;
      }
      uVar4 = uVar4 + iVar5;
      goto LAB_ram_0000cba0;
    }
    bVar3 = param_1[2] << 4;
    if (param_1[1] == 1) {
LAB_ram_0000cbb6:
      bVar3 = bVar3 | 0x80;
    }
  }
  (&DAT_ram_e000e400)[bVar1] = bVar3;
LAB_ram_0000cbc8:
  if (*(int *)(param_1 + 4) == 0) {
    iVar5 = (bVar1 >> 5) + 0x60;
  }
  else {
    iVar5 = (bVar1 >> 5) + 0x40;
  }
  *(int *)(&PFIC + iVar5 * 4) = 1 << (bVar1 & 0x1f);
  return;
}

