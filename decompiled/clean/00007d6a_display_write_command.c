/* Semantic review: high. PC5 low PB output word PC13 write strobe PC4 select.
 * Address 0x00007d6a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007d6a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: PC5 low PB output word PC13 write strobe PC4 select */

void display_write_command(ushort command)

{
  gp = &__global_pointer;
  GPIO_WriteBit(&GPIOC,0x20,0);
  GPIO_WriteBit(&GPIOC,0x10,0);
  GPIOB.OUTDR = (uint)command;
  GPIO_WriteBit(&GPIOC,0x2000,0);
  GPIO_WriteBit(&GPIOC,0x2000,1);
  GPIO_WriteBit(&GPIOC,0x10,1);
  return;
}

