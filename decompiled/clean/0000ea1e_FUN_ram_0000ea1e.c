/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000ea1e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000ea1e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

uint FUN_ram_0000ea1e(char *param_1)

{
  int iVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  if ((param_1[3] == '\0') || (uVar2 = FUN_ram_0000e134(), uVar2 == 0)) {
    if ((*param_1 == '\x03') && (param_1[4] == '\x01')) {
      memset_recovered(param_1 + 0x38,0,0x1000);
      param_1[0x236] = 'U';
      param_1[0x237] = -0x56;
      param_1[0x38] = 'R';
      param_1[0x39] = 'R';
      param_1[0x3a] = 'a';
      param_1[0x3b] = 'A';
      param_1[0x21c] = 'r';
      param_1[0x21d] = 'r';
      param_1[0x21e] = 'A';
      param_1[0x21f] = 'a';
      FUN_ram_0000de70(param_1 + 0x220,*(undefined4 *)(param_1 + 0x18));
      FUN_ram_0000de70(param_1 + 0x224,*(undefined4 *)(param_1 + 0x14));
      *(int *)(param_1 + 0x34) = *(int *)(param_1 + 0x24) + 1;
      disk_write(param_1[1],param_1 + 0x38,*(int *)(param_1 + 0x24) + 1,1);
      param_1[4] = '\0';
    }
    iVar1 = disk_ioctl(param_1[1],0,0);
    uVar2 = (uint)(iVar1 != 0);
  }
  return uVar2;
}

