/* Semantic review: high. Byte loop ending at zero and return length.
 * Address 0x000090d2; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000090d2; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Byte loop ending at zero and return length */

size_t strlen(char *__s)

{
  size_t sVar1;
  
  gp = &__global_pointer;
  for (sVar1 = 0; __s[sVar1] != '\0'; sVar1 = sVar1 + 1) {
  }
  return sVar1;
}

