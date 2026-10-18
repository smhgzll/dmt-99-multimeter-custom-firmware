/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000e5e8; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000e5e8; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

int FUN_ram_0000e5e8(char *param_1,uint param_2,uint param_3)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  byte bVar4;
  byte bVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  if (param_2 < 2) {
    gp = &__global_pointer;
    return 2;
  }
  if (*(uint *)(param_1 + 0x1c) <= param_2) {
    gp = &__global_pointer;
    return 2;
  }
  cVar1 = *param_1;
  bVar5 = (byte)param_3;
  bVar4 = (byte)(param_3 >> 8);
  if (cVar1 == '\x02') {
    iVar3 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) +
                                     param_2 / (*(ushort *)(param_1 + 0xc) >> 1));
    if (iVar3 != 0) {
      gp = &__global_pointer;
      return iVar3;
    }
    uVar2 = *(ushort *)(param_1 + 0xc);
    param_1[(param_2 << 1) % (uint)uVar2 + 0x38] = bVar5;
    (param_1 + (param_2 << 1) % (uint)uVar2 + 0x38)[1] = bVar4;
  }
  else {
    if (cVar1 != '\x03') {
      if (cVar1 != '\x01') {
        gp = &__global_pointer;
        return 2;
      }
      uVar6 = (param_2 >> 1) + param_2;
      iVar3 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) + uVar6 / *(ushort *)(param_1 + 0xc)
                              );
      if (iVar3 != 0) {
        gp = &__global_pointer;
        return iVar3;
      }
      if ((param_2 & 1) != 0) {
        bVar5 = (byte)(param_3 << 4) |
                param_1[uVar6 % (uint)*(ushort *)(param_1 + 0xc) + 0x38] & 0xfU;
      }
      param_1[uVar6 % (uint)*(ushort *)(param_1 + 0xc) + 0x38] = bVar5;
      param_1[3] = '\x01';
      iVar3 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) +
                                       (uVar6 + 1) / (uint)*(ushort *)(param_1 + 0xc));
      if (iVar3 != 0) {
        gp = &__global_pointer;
        return iVar3;
      }
      if ((param_2 & 1) == 0) {
        bVar4 = bVar4 & 0xf | param_1[(uVar6 + 1) % (uint)*(ushort *)(param_1 + 0xc) + 0x38] & 0xf0U
        ;
      }
      else {
        bVar4 = (byte)(param_3 >> 4);
      }
      param_1[(uVar6 + 1) % (uint)*(ushort *)(param_1 + 0xc) + 0x38] = bVar4;
      param_1[3] = '\x01';
      gp = &__global_pointer;
      return 0;
    }
    iVar3 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) +
                                     param_2 / (*(ushort *)(param_1 + 0xc) >> 2));
    if (iVar3 != 0) {
      gp = &__global_pointer;
      return iVar3;
    }
    uVar2 = *(ushort *)(param_1 + 0xc);
    uVar6 = FUN_ram_0000de5a(param_1 + (param_2 << 2) % (uint)uVar2 + 0x38);
    FUN_ram_0000de70(param_1 + (param_2 << 2) % (uint)uVar2 + 0x38,
                     param_3 & 0xfffffff | uVar6 & 0xf0000000);
  }
  param_1[3] = '\x01';
  return 0;
}

