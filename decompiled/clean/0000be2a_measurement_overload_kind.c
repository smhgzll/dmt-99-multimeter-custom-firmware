/* Semantic review: high. String positions distinguish two OL representations returning 1 or 2.
 * Address 0x0000be2a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000be2a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: String positions distinguish two OL representations returning 1 or 2 */

uint measurement_overload_kind(void)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if (*(char *)(measurement_text_pointer + 2) == '.') {
    uVar1 = 0;
    if (*(char *)(measurement_text_pointer + 3) == '0') {
      return (uint)(*(char *)(measurement_text_pointer + 4) == 'L');
    }
  }
  else {
    uVar1 = 0;
    if (((*(char *)(measurement_text_pointer + 2) == '0') &&
        (*(char *)(measurement_text_pointer + 3) == '.')) &&
       (*(char *)(measurement_text_pointer + 4) == 'L')) {
      uVar1 = 2;
    }
  }
  return uVar1;
}

