/* Semantic review: high. Encoded USART interrupt register selection.
 * Address 0x0000d168; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d168; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Encoded USART interrupt register selection */

void USART_ITConfig(int param_1,uint param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  gp = &__global_pointer;
  uVar2 = (param_2 & 0xff) >> 5;
  uVar1 = 1 << (param_2 & 0x1f);
  if (uVar2 == 1) {
    puVar3 = (uint *)(param_1 + 0xc);
  }
  else {
    puVar3 = (uint *)(param_1 + 0x14);
    if (uVar2 == 2) {
      puVar3 = (uint *)(param_1 + 0x10);
    }
  }
  if (param_3 == 0) {
    uVar1 = ~uVar1 & *puVar3;
  }
  else {
    uVar1 = uVar1 | *puVar3;
  }
  *puVar3 = uVar1;
  return;
}

