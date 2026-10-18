/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000a7d0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000a7d0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

int FUN_ram_0000a7d0(void)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  iVar1 = 0;
  if (display_layout == '\0') {
    iVar1 = 0x74;
  }
  if (measurement_ac_dc_code == '\x02') {
    if (DAT_ram_200001d9 != '\x05') {
      DAT_ram_200001d9 = 5;
      iVar2 = settings_language_access(0,0);
      if (iVar2 == 0) {
        if (measurement_mode != '\x14') {
          gp = &__global_pointer;
          return 0;
        }
        iVar1 = 0x96 - iVar1;
        uVar4 = 0x6b;
        pcVar3 = "mode/dc_v_c.jpg";
      }
      else {
        if (measurement_mode != '\x14') {
          gp = &__global_pointer;
          return iVar2;
        }
        iVar1 = 0x96 - iVar1;
        uVar4 = 0x77;
        pcVar3 = "mode/dc_v.jpg";
      }
LAB_ram_0000a822:
      iVar1 = resource_draw_jpeg(pcVar3,uVar4,iVar1);
      gp = &__global_pointer;
      return iVar1;
    }
  }
  else if (measurement_ac_dc_code == '\x01') {
    if (DAT_ram_200001d9 != '\x04') {
      DAT_ram_200001d9 = 4;
      iVar2 = settings_language_access(0,0);
      if (iVar2 == 0) {
        if (measurement_mode != '\x14') {
          gp = &__global_pointer;
          return 0;
        }
        iVar1 = 0x96 - iVar1;
        uVar4 = 0x6b;
        pcVar3 = "mode/ac_v_c.jpg";
      }
      else {
        if (measurement_mode != '\x14') {
          gp = &__global_pointer;
          return iVar2;
        }
        iVar1 = 0x96 - iVar1;
        uVar4 = 0x77;
        pcVar3 = "mode/ac_v.jpg";
      }
      goto LAB_ram_0000a822;
    }
  }
  else if ((byte)(measurement_unit_code - 0xbU) < 3) {
    if (DAT_ram_200001d9 != '\x0e') {
      DAT_ram_200001d9 = 0xe;
      iVar2 = settings_language_access(0,0);
      iVar1 = 0x96 - iVar1;
      if (iVar2 == 0) {
        uVar4 = 0x6b;
        pcVar3 = "mode/r_c.jpg";
      }
      else {
        uVar4 = 0x77;
        pcVar3 = "mode/r.jpg";
      }
      goto LAB_ram_0000a822;
    }
  }
  else if (DAT_ram_200001d9 != -1) {
    DAT_ram_200001d9 = 0xff;
    iVar1 = display_fill_rectangle(0x4a,0x93U - iVar1 & 0xffff,0xf1,0xaeU - iVar1 & 0xffff,0);
    gp = &__global_pointer;
    return iVar1;
  }
  return 0;
}

