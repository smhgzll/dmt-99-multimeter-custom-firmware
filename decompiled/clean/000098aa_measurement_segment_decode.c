/* Semantic review: high. Masks 0x10 then matches explicit seven-segment patterns.
 * Address 0x000098aa; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000098aa; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Masks 0x10 then matches explicit seven-segment patterns */

uint measurement_segment_decode(uint segments)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar2 = segments & 0xef;
  if (uVar2 == 0xeb) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
    if (((((((uVar2 != 10) && (uVar1 = 2, uVar2 != 0xad)) && (uVar1 = 3, uVar2 != 0x8f)) &&
          ((uVar1 = 4, uVar2 != 0x4e && (uVar1 = 5, uVar2 != 199)))) &&
         (((uVar1 = 6, uVar2 != 0xe7 && ((uVar1 = 7, uVar2 != 0x8a && (uVar1 = 8, uVar2 != 0xef))))
          && (uVar1 = 9, uVar2 != 0xcf)))) &&
        ((((uVar1 = 10, uVar2 != 0xee && (uVar1 = 0xb, uVar2 != 0x23)) &&
          (uVar1 = 0xc, uVar2 != 0x65)) &&
         (((uVar1 = 0xd, uVar2 != 0x27 && (uVar1 = 0xe, uVar2 != 0x61)) &&
          ((uVar1 = 0xf, uVar2 != 0xe5 && ((uVar1 = 0x10, uVar2 != 4 && (uVar1 = 0x11, uVar2 != 0)))
           ))))))) &&
       ((uVar1 = 0x12, uVar2 != 0xe1 &&
        ((uVar1 = 0x13, uVar2 != 0xec && (uVar1 = 0x15, uVar2 != 0x24)))))) {
      return 0xff;
    }
  }
  return uVar1;
}

