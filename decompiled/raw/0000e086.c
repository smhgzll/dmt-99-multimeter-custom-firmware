/* Original address: 0x0000e086; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

int FUN_ram_0000e086(undefined4 *param_1)

{
  byte *pbVar1;
  uint uVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  
  gp = &__global_pointer;
  pbVar4 = (byte *)*param_1;
  uVar5 = (uint)*pbVar4;
  pbVar1 = pbVar4 + 1;
  if (uVar5 < 0x81) {
    iVar3 = 0;
    if (uVar5 != 0) goto LAB_ram_0000e0dc;
LAB_ram_0000e0ee:
    *param_1 = pbVar1;
  }
  else {
    if (uVar5 == 0xff) {
      uVar5 = 0xff;
LAB_ram_0000e0dc:
      iVar3 = FUN_ram_0000fcfc(uVar5,0x3a8);
      if (iVar3 != 0) goto LAB_ram_0000e0ee;
    }
    else {
      pbVar1 = pbVar4 + 2;
      uVar2 = (uint)pbVar4[1];
      if ((0x3f < uVar2) && ((uVar2 < 0x7f || (((char)pbVar4[1] < '\0' && (uVar2 != 0xff)))))) {
        uVar5 = uVar5 * 0x100 + uVar2;
        goto LAB_ram_0000e0dc;
      }
    }
    iVar3 = -1;
  }
  return iVar3;
}

