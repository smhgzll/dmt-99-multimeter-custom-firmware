/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000e7fc; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000e7fc; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

undefined4 FUN_ram_0000e7fc(undefined4 *param_1,uint param_2)

{
  ushort uVar1;
  ushort uVar2;
  byte *pbVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  if (0x1fffff < param_2) {
    return 2;
  }
  if ((param_2 & 0x1f) == 0) {
    uVar6 = param_1[2];
    pbVar3 = (byte *)*param_1;
    param_1[4] = param_2;
    uVar5 = uVar6;
    if ((uVar6 == 0) && ((*pbVar3 < 3 || (uVar5 = *(uint *)(pbVar3 + 0x2c), uVar5 == 0)))) {
      if ((uint)*(ushort *)(pbVar3 + 8) <= param_2 >> 5) {
        return 2;
      }
      param_1[6] = *(undefined4 *)(pbVar3 + 0x2c);
      uVar5 = uVar6;
    }
    else {
      uVar1 = *(ushort *)(pbVar3 + 10);
      uVar2 = *(ushort *)(pbVar3 + 0xc);
      for (; (uint)uVar1 * (uint)uVar2 <= param_2; param_2 = param_2 - (uint)uVar1 * (uint)uVar2) {
        uVar5 = FUN_ram_0000e71e(*param_1,uVar5);
        if (uVar5 == 0xffffffff) {
          return 1;
        }
        if (uVar5 < 2) {
          return 2;
        }
        if (*(uint *)(pbVar3 + 0x1c) <= uVar5) {
          return 2;
        }
      }
      uVar4 = FUN_ram_0000de84(pbVar3);
      param_1[6] = uVar4;
    }
    param_1[5] = uVar5;
    if (param_1[6] != 0) {
      uVar1 = *(ushort *)(pbVar3 + 0xc);
      param_1[6] = param_2 / uVar1 + param_1[6];
      param_1[7] = pbVar3 + param_2 % (uint)uVar1 + 0x38;
      return 0;
    }
  }
  return 2;
}

