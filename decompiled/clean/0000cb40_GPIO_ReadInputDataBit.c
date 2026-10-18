/* Semantic review: high. GPIO INDR masked boolean read.
 * Address 0x0000cb40; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb40; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO INDR masked boolean read */

uint GPIO_ReadInputDataBit(GPIO_Registers *port,ushort pin)

{
  gp = &__global_pointer;
  return (uint)((port->INDR & (uint)pin) != 0);
}

