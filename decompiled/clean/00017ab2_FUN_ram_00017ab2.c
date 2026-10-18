/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00017ab2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00017ab2; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_00017ab2(uint param_1,uint param_2,undefined2 param_3)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if ((param_2 < 0x14) && (DAT_ram_20000254 == '\0')) {
    if (param_1 < 0xc4) {
      (&DAT_ram_20003634)[param_1 * 0x14 + param_2] = param_3;
      return;
    }
  }
  else {
    uVar1 = param_2 - 0x14 & 0xffff;
    if (((((uVar1 < 0x14) && (DAT_ram_20000254 == '\x01')) ||
         ((uVar1 = param_2 - 0x28 & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\x02')))) ||
        ((uVar1 = param_2 - 0x3c & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\x03')))) ||
       (((((uVar1 = param_2 - 0x50 & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\x04')) ||
          ((uVar1 = param_2 - 100 & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\x05')))) ||
         ((uVar1 = param_2 - 0x78 & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\x06')))) ||
        (((uVar1 = param_2 - 0x8c & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\a')) ||
         ((uVar1 = param_2 - 0xa0 & 0xffff, uVar1 < 0x14 && (DAT_ram_20000254 == '\b')))))))) {
      if (param_1 < 0xc4) {
        (&DAT_ram_20003634)[param_1 * 0x14 + uVar1] = param_3;
        return;
      }
    }
    else {
      uVar1 = param_2 - 0xb4 & 0xffff;
      if ((uVar1 < 0x14) && ((DAT_ram_20000254 == '\t' && (param_1 < 0xc4)))) {
        (&DAT_ram_20003634)[param_1 * 0x14 + uVar1] = param_3;
      }
    }
  }
  return;
}

