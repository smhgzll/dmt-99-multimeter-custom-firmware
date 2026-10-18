/* Original address: 0x0000c8f0; Ghidra pseudocode, not compilable reconstructed source. */

/* high: EXTI rising falling and interrupt-event mode masks */

void EXTI_Init(uint *param_1)

{
  uint uVar1;
  uint *puVar2;
  uint uVar3;
  
  gp = &__global_pointer;
  puVar2 = (uint *)((int)&EXTI + param_1[1]);
  uVar1 = ~*param_1;
  if (param_1[3] == 0) {
    *puVar2 = *puVar2 & uVar1;
    return;
  }
  uVar3 = EXTI;
  EXTI = uVar3 & uVar1;
  uVar3 = DAT_ram_40010404;
  DAT_ram_40010404 = uVar1 & uVar3;
  *puVar2 = *puVar2 | *param_1;
  uVar3 = *param_1;
  uVar1 = DAT_ram_40010408;
  DAT_ram_40010408 = uVar1 & ~uVar3;
  uVar1 = DAT_ram_4001040c;
  DAT_ram_4001040c = ~uVar3 & uVar1;
  if (param_1[2] == 0x10) {
    uVar1 = DAT_ram_40010408;
    DAT_ram_40010408 = uVar1 | uVar3;
    uVar1 = DAT_ram_4001040c;
    DAT_ram_4001040c = uVar3 | uVar1;
    return;
  }
  puVar2 = (uint *)((int)&EXTI + param_1[2]);
  *puVar2 = uVar3 | *puVar2;
  return;
}

