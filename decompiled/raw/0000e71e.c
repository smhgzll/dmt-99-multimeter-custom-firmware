/* Original address: 0x0000e71e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

uint FUN_ram_0000e71e(char *param_1,uint param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  if (param_2 < 2) {
    return 1;
  }
  if (*(uint *)(param_1 + 0x1c) <= param_2) {
    gp = &__global_pointer;
    return 1;
  }
  cVar1 = *param_1;
  if (cVar1 == '\x02') {
    iVar2 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) +
                                     param_2 / (*(ushort *)(param_1 + 0xc) >> 1));
    if (iVar2 == 0) {
      gp = &__global_pointer;
      return (uint)*(ushort *)(param_1 + (param_2 << 1) % (uint)*(ushort *)(param_1 + 0xc) + 0x38);
    }
  }
  else if (cVar1 == '\x03') {
    iVar2 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) +
                                     param_2 / (*(ushort *)(param_1 + 0xc) >> 2));
    if (iVar2 == 0) {
      iVar2 = FUN_ram_0000de5a(param_1 + (param_2 << 2) % (uint)*(ushort *)(param_1 + 0xc) + 0x38);
      uVar3 = iVar2 << 4;
LAB_ram_0000e798:
      gp = &__global_pointer;
      return uVar3 >> 4;
    }
  }
  else {
    if (cVar1 != '\x01') {
      gp = &__global_pointer;
      return 1;
    }
    uVar3 = (param_2 >> 1) + param_2;
    iVar2 = FUN_ram_0000e17c(param_1,*(int *)(param_1 + 0x28) + uVar3 / *(ushort *)(param_1 + 0xc));
    if (iVar2 == 0) {
      cVar1 = param_1[uVar3 % (uint)*(ushort *)(param_1 + 0xc) + 0x38];
      iVar2 = FUN_ram_0000e17c(param_1,(uVar3 + 1) / (uint)*(ushort *)(param_1 + 0xc) +
                                       *(int *)(param_1 + 0x28));
      if (iVar2 == 0) {
        uVar3 = (uint)CONCAT11(param_1[(uVar3 + 1) % (uint)*(ushort *)(param_1 + 0xc) + 0x38],cVar1)
        ;
        if ((param_2 & 1) == 0) {
          gp = &__global_pointer;
          return uVar3 & 0xfff;
        }
        goto LAB_ram_0000e798;
      }
    }
  }
  return 0xffffffff;
}

