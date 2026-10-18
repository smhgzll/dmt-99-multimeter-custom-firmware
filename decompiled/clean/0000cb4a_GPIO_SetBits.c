/* Semantic review: high. GPIO BSHR write.
 * Address 0x0000cb4a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb4a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO BSHR write */

void GPIO_SetBits(GPIO_Registers *port,ushort pins)

{
  gp = &__global_pointer;
  port->BSHR = (uint)pins;
  return;
}

