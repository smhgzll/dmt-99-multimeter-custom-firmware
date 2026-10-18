/* Original address: 0x0000e1bc; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000e1bc(int param_1)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  
  gp = &__global_pointer;
  *(undefined4 *)(param_1 + 0x34) = 0xffffffff;
  *(undefined1 *)(param_1 + 3) = 0;
  iVar4 = FUN_ram_0000e17c();
  uVar6 = 4;
  if (iVar4 == 0) {
    sVar2 = *(short *)(param_1 + 0x236);
    if ((*(char *)(param_1 + 0x38) == -0x15) || ((byte)(*(char *)(param_1 + 0x38) + 0x18U) < 2)) {
      if ((sVar2 == -0x55ab) && (iVar4 = FUN_ram_0001e57a(param_1 + 0x8a,"FAT32   ",8), iVar4 == 0))
      {
        gp = &__global_pointer;
        return 0;
      }
      uVar3 = *(ushort *)(param_1 + 0x43);
      if (((((((ushort)(uVar3 - 0x200) < 0xe01) && (bVar1 = *(byte *)(param_1 + 0x45), bVar1 != 0))
            && ((uVar3 & uVar3 - 1) == 0 && (bVar1 - 1 & bVar1) == 0)) &&
           ((*(short *)(param_1 + 0x46) != 0 && (*(byte *)(param_1 + 0x48) - 1 < 2)))) &&
          ((*(short *)(param_1 + 0x49) != 0 &&
           ((0x7f < *(ushort *)(param_1 + 0x4b) ||
            (uVar5 = FUN_ram_0000de5a(param_1 + 0x58), 0xffff < uVar5)))))) &&
         (*(short *)(param_1 + 0x4e) != 0)) {
        gp = &__global_pointer;
        return 0;
      }
    }
    uVar6 = 2;
    if (sVar2 != -0x55ab) {
      uVar6 = 3;
    }
  }
  return uVar6;
}

