/* Semantic review: high. Vector slot 45; TIM3 update flag and periodic power management.
 * Address 0x00007780; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00007780; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 45; TIM3 update flag and periodic power management */

void TIM3_IRQHandler(void)

{
  int iVar1;
  uint uVar2;
  char cVar3;
  byte bVar4;
  
  gp = &__global_pointer;
  iVar1 = TIM_GetITStatus(&TIM3,1);
  if (iVar1 != 0) {
    TIM_ClearITPendingBit(&TIM3,1);
    cVar3 = periodic_tick_count + '\x01';
    bVar4 = periodic_tick_count + 1;
    periodic_tick_count = cVar3;
    if (0x14 < bVar4) {
      iVar1 = battery_percentage_estimate();
      if ((iVar1 < 1) && (uVar2 = GPIO_ReadInputDataBit(&GPIOA,0x100), uVar2 == 0)) {
        FUN_ram_00014c42();
      }
      battery_refresh_due = 1;
      periodic_tick_count = '\0';
      uVar2 = settings_auto_off_access(0,0);
      if ((uVar2 & 0xffff) != 0) {
        idle_seconds_counter = idle_seconds_counter + 0x14;
        if (auto_off_seconds < idle_seconds_counter) {
                    /* WARNING: Subroutine does not return */
          power_down();
        }
      }
    }
  }
  return;
}

