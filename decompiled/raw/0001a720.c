/* Original address: 0x0001a720; Ghidra pseudocode, not compilable reconstructed source. */

undefined * FUN_ram_0001a720(int param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  gp = &__global_pointer;
  puVar1 = PTR_DAT_ram_200054d8_ram_200001e0 + param_1;
  if ((puVar1 < &DAT_ram_200054d8) ||
     (puVar2 = PTR_DAT_ram_200054d8_ram_200001e0, (undefined *)0x2000f800 < puVar1)) {
    puVar2 = (undefined *)0xffffffff;
    puVar1 = PTR_DAT_ram_200054d8_ram_200001e0;
  }
  PTR_DAT_ram_200054d8_ram_200001e0 = puVar1;
  return puVar2;
}

