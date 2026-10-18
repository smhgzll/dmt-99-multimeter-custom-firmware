/* Original address: 0x00008016; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00008016(void)

{
  uint uVar1;
  undefined4 uVar2;
  
  gp = &__global_pointer;
  uVar1 = settings_field_3_4_access(0,0);
  uVar2 = 0x32;
  if (uVar1 != 1) {
    uVar1 = settings_field_3_4_access(0,0);
    uVar2 = 0;
    if (uVar1 == 2) {
      uVar2 = 0x46;
    }
  }
  tim8_backlight_pwm_config(100,0x12bf,uVar2);
  return;
}

