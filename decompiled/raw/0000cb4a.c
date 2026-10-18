/* Original address: 0x0000cb4a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO BSHR write */

void GPIO_SetBits(GPIO_Registers *port,ushort pins)

{
  gp = &__global_pointer;
  port->BSHR = (uint)pins;
  return;
}

