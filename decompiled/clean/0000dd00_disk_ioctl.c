/* Semantic review: high. Sector count 2048 sector size 4096 erase block 1.
 * Address 0x0000dd00; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000dd00; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Sector count 2048 sector size 4096 erase block 1 */

undefined4 disk_ioctl(int param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  
  gp = &__global_pointer;
  if (param_1 == 0) {
    if (param_2 == 2) goto LAB_ram_0000dd26;
    if (param_2 != 3) {
      if (param_2 != 1) {
        return 0;
      }
      goto LAB_ram_0000dd1c;
    }
  }
  else {
    if (param_1 != 1) {
      return 4;
    }
    if (param_2 == 2) {
LAB_ram_0000dd26:
      *(undefined2 *)param_3 = 0x1000;
      return 0;
    }
    if (param_2 != 3) {
      if (param_2 != 1) {
        return 0;
      }
LAB_ram_0000dd1c:
      uVar1 = 0x800;
      goto LAB_ram_0000dd22;
    }
  }
  uVar1 = 1;
LAB_ram_0000dd22:
  *param_3 = uVar1;
  return 0;
}

