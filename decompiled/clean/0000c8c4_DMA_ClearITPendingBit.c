/* Semantic review: high. DMA interrupt-clear registers selected by encoded flag.
 * Address 0x0000c8c4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c8c4; Ghidra pseudocode, not compilable reconstructed source. */

/* high: DMA interrupt-clear registers selected by encoded flag */

void DMA_ClearITPendingBit(int param_1)

{
  gp = &__global_pointer;
  if (param_1 << 3 < 0) {
    DAT_ram_40020404 = param_1;
    return;
  }
  if (param_1 << 2 < 0) {
    DAT_ram_400204d4 = param_1;
    return;
  }
  DAT_ram_40020004 = param_1;
  return;
}

