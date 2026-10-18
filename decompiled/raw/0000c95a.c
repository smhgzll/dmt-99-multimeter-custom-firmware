/* Original address: 0x0000c95a; Ghidra pseudocode, not compilable reconstructed source. */

/* high: EXTI enable plus pending tests */

bool EXTI_GetITStatus(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  gp = &__global_pointer;
  uVar1 = EXTI;
  uVar2 = DAT_ram_40010414;
  if ((uVar2 & param_1) != 0) {
    return (param_1 & uVar1) != 0;
  }
  return false;
}

