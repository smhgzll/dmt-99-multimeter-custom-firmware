/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00018974; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00018974; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_00018974(void)

{
  int iVar1;
  undefined4 uVar2;
  char *pcVar3;
  uint uVar4;
  
  gp = &__global_pointer;
  iVar1 = 0;
  if (display_layout == '\0') {
    iVar1 = 10;
  }
  uVar2 = FUN_ram_0000a7a4();
  if (measurement_mode == '\f') {
    if (DAT_ram_200001da == '\a') goto LAB_ram_000189ce;
    DAT_ram_200001da = 7;
    display_fill_rectangle(0x102,iVar1 + 0x9e,0x120,iVar1 + 0xb1,0);
    pcVar3 = "units/v.jpg";
  }
  else if ((measurement_mode == '\x10') || (measurement_mode == '\x15')) {
    if (measurement_unit_code == '\t') {
      if (DAT_ram_200001da == '\n') goto LAB_ram_000189ce;
      DAT_ram_200001da = 10;
      display_fill_rectangle(0x102,iVar1 + 0x9e,0x120,iVar1 + 0xb1,0);
      pcVar3 = "units/ma.jpg";
    }
    else {
      if (measurement_unit_code != '\n') {
        gp = &__global_pointer;
        return uVar2;
      }
      if (DAT_ram_200001da == '\t') {
LAB_ram_000189ce:
        gp = &__global_pointer;
        return 0;
      }
      DAT_ram_200001da = 9;
      display_fill_rectangle(0x102,iVar1 + 0x9e,0x120,iVar1 + 0xb1,0);
      pcVar3 = "units/a.jpg";
    }
  }
  else {
    if (measurement_mode != '\x12') {
      gp = &__global_pointer;
      return uVar2;
    }
    if (measurement_unit_code != '\x15') {
      gp = &__global_pointer;
      return uVar2;
    }
    if (DAT_ram_200001da == '\x16') goto LAB_ram_000189ce;
    DAT_ram_200001da = 0x16;
    display_fill_rectangle(0x102,iVar1 + 0x9e,0x120,iVar1 + 0xb1,0);
    uVar4 = settings_temperature_access(0,0);
    if (uVar4 == 0) {
      pcVar3 = "units/c.jpg";
    }
    else {
      pcVar3 = "units/f.jpg";
    }
  }
  uVar2 = resource_draw_jpeg(pcVar3,0x102,iVar1 + 0x9e);
  return uVar2;
}

