/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000fd46; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000fd46; Ghidra pseudocode, not compilable reconstructed source. */

uint FUN_ram_0000fd46(uint param_1)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  ushort *puVar4;
  ushort *puVar5;
  uint uVar6;
  
  gp = &__global_pointer;
  if (0xffff < param_1) {
    return param_1;
  }
  uVar6 = param_1 & 0xffff;
  if (param_1 < 0x1000) {
    puVar5 = &DAT_ram_00023d38;
  }
  else {
    puVar5 = &DAT_ram_00023f2c;
  }
  while( true ) {
    uVar3 = (uint)*puVar5;
    if (uVar3 == 0) {
      return uVar6;
    }
    if (uVar6 < uVar3) {
      return uVar6;
    }
    puVar4 = puVar5 + 2;
    uVar1 = puVar5[1] >> 8;
    uVar2 = puVar5[1] & 0xff;
    if ((int)param_1 < (int)(uVar2 + uVar3)) break;
    puVar5 = puVar4;
    if (uVar1 == 0) {
      puVar5 = puVar4 + uVar2;
    }
  }
  switch(uVar1) {
  case 0:
    return (uint)puVar4[param_1 - uVar3];
  case 1:
    uVar6 = uVar6 - (uVar6 - uVar3 & 1);
    break;
  case 2:
    uVar6 = uVar6 - 0x10;
    break;
  case 3:
    uVar6 = uVar6 - 0x20;
    break;
  case 4:
    uVar6 = uVar6 - 0x30;
    break;
  case 5:
    uVar6 = uVar6 - 0x1a;
    break;
  case 6:
    uVar6 = uVar6 + 8;
    break;
  case 7:
    uVar6 = uVar6 - 0x50;
    break;
  case 8:
    uVar6 = uVar6 - 0x1c60;
    break;
  default:
    goto switchD_ram_0000fd92_default;
  }
  uVar6 = uVar6 & 0xffff;
switchD_ram_0000fd92_default:
  return uVar6;
}

