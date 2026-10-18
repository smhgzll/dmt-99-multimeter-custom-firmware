/* Semantic review: high. GPIO BCR write.
 * Address 0x0000cb4e; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb4e; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO BCR write */

void GPIO_ResetBits(GPIO_Registers *port,ushort pins)

{
  gp = &__global_pointer;
  port->BCR = (uint)pins;
  return;
}

