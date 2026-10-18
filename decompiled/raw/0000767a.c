/* Original address: 0x0000767a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: ADC1 initialization calibration and PC0 analog input */

void adc_battery_init(void)

{
  int iVar1;
  GPIO_Init_Recovered GStack_3c;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined1 uStack_1c;
  
  gp = &__global_pointer;
  RCC_APB2PeriphClockCmd(0x210,1);
  RCC_ADCCLKConfig(0xc000);
  GStack_3c.pin = 1;
  GStack_3c.speed = 3;
  GStack_3c.mode = 0;
  GPIO_Init(&GPIOC,&GStack_3c);
  uStack_1c = 1;
  uStack_24 = 0xe0000;
  uStack_30 = 0;
  uStack_2c = 0;
  uStack_28 = 0;
  uStack_20 = 0;
  ADC_Init(&ADC1,&uStack_30);
  ADC_Cmd(&ADC1,1);
  ADC_ResetCalibration(&ADC1);
  do {
    iVar1 = ADC_GetResetCalibrationStatus(&ADC1);
  } while (iVar1 != 0);
  ADC_StartCalibration(&ADC1);
  do {
    iVar1 = ADC_GetCalibrationStatus(&ADC1);
  } while (iVar1 != 0);
  ADC_SoftwareStartConvCmd(&ADC1,1);
  return;
}

