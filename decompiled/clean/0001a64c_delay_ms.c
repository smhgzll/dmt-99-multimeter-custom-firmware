/* Semantic review: high. SysTick compare count enable and status polling.
 * Address 0x0001a64c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0001a64c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SysTick compare count enable and status polling */

void delay_ms(uint milliseconds)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = DAT_ram_e000f004;
  DAT_ram_e000f004 = uVar1 & 0xfffffffe;
  DAT_ram_e000f010 = DAT_ram_20000262 * milliseconds;
  DAT_ram_e000f014 = 0;
  uVar1 = SysTick;
  SysTick = uVar1 | 0x10;
  uVar1 = SysTick;
  SysTick = uVar1 | 0x21;
  do {
    uVar1 = DAT_ram_e000f004;
  } while ((uVar1 & 1) == 0);
  uVar1 = SysTick;
  SysTick = uVar1 & 0xfffffffe;
  return;
}

