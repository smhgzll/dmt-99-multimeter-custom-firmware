/* Original address: 0x00007736; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: TIM3 base values 9999 and 14399; update interrupt setup */

void tim3_periodic_init(undefined2 param_1,undefined2 param_2)

{
  undefined2 uStack_1c;
  undefined2 uStack_1a;
  undefined2 uStack_18;
  undefined2 uStack_16;
  
  gp = &__global_pointer;
  RCC_APB1PeriphClockCmd(2,1);
  FUN_ram_00007716();
  uStack_1a = 0;
  uStack_16 = 0;
  uStack_1c = param_2;
  uStack_18 = param_1;
  TIM_TimeBaseInit(&TIM3,&uStack_1c);
  TIM_ITConfig(&TIM3,1,1);
  TIM_Cmd(&TIM3,1);
  return;
}

