/* Semantic review: high. SPI CTLR1 enable bit.
 * Address 0x0000cdea; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cdea; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI CTLR1 enable bit */

void SPI_Cmd(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xffbf;
  }
  else {
    uVar1 = *param_1 | 0x40;
  }
  *param_1 = uVar1;
  return;
}

