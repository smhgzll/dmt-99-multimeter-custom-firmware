/* Semantic review: medium. ADC channel 10 and voltage diagnostics; float conversion signatures unresolved.
 * Address 0x000075f0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000075f0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* medium: ADC channel 10 and voltage diagnostics; float conversion signatures unresolved */

void battery_percentage_estimate(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  
  gp = &__global_pointer;
  iVar1 = adc_read_averaged(10,0x32);
  uVar2 = FUN_ram_0001e21c(iVar1 * 0x19c8 >> 0xc);
  uVar3 = FUN_ram_0001dddc(0x457a0000,uVar2);
  uVar3 = FUN_ram_0001d728(uVar3,0x44160000);
  uVar3 = FUN_ram_0001dddc(0x3f800000,uVar3);
  uVar4 = FUN_ram_0001e2e2(uVar2);
  uVar2 = (undefined4)((ulonglong)uVar4 >> 0x20);
  FUN_ram_0001aede("voltage:%f\r\n",uVar2,(int)uVar4,uVar2);
  uVar4 = FUN_ram_0001e2e2(uVar3);
  uVar2 = (undefined4)((ulonglong)uVar4 >> 0x20);
  FUN_ram_0001aede(&DAT_ram_0001ed90,uVar2,(int)uVar4,uVar2);
  FUN_ram_0001db48(uVar3,0x42c80000);
  FUN_ram_0001e164();
  return;
}

