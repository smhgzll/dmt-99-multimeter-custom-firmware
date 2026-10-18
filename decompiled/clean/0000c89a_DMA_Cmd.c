/* Semantic review: high. DMA CFGR enable bit.
 * Address 0x0000c89a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c89a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: DMA CFGR enable bit */

void DMA_Cmd(uint *param_1,int param_2)

{
  uint uVar1;
  
  gp = &__global_pointer;
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xfffe;
  }
  else {
    uVar1 = *param_1 | 1;
  }
  *param_1 = uVar1;
  return;
}

