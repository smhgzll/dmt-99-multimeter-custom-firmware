/* Semantic review: high. AFIO external interrupt port selector nibble.
 * Address 0x0000cb5c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb5c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: AFIO external interrupt port selector nibble */

void GPIO_EXTILineConfig(int param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  
  gp = &__global_pointer;
  uVar1 = param_2 & 0xfc;
  iVar2 = (param_2 & 3) << 2;
  *(uint *)(&DAT_ram_40010008 + uVar1) = ~(0xf << iVar2) & *(uint *)(&DAT_ram_40010008 + uVar1);
  *(uint *)(&DAT_ram_40010008 + uVar1) = param_1 << iVar2 | *(uint *)(&DAT_ram_40010008 + uVar1);
  return;
}

