/* Original address: 0x0000cb52; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO BSHR or BCR according to bit action */

void GPIO_WriteBit(GPIO_Registers *port,ushort pin,uint high)

{
  gp = &__global_pointer;
  if (high != 0) {
    port->BSHR = (uint)pin;
    return;
  }
  port->BCR = (uint)pin;
  return;
}

