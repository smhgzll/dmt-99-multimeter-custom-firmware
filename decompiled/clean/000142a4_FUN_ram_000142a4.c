/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000142a4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000142a4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_000142a4(int param_1,int param_2,int param_3)

{
  char *pcVar1;
  short value;
  undefined4 uVar2;
  
  gp = &__global_pointer;
  uVar2 = 0x37;
  if (param_2 == 2) {
    uVar2 = 0x98;
  }
  if ((param_3 - 1U & 0xff) < 2) {
    if (param_3 == 1) {
      if (param_1 != 0) {
        resource_draw_jpeg("menu/lighting2_.jpg",0x8c,uVar2);
        value = 1;
LAB_ram_000142ea:
        settings_field_3_4_access(1,value);
        gp = &__global_pointer;
        return;
      }
      pcVar1 = "menu/lighting2.jpg";
    }
    else {
      if (param_1 != 0) {
        resource_draw_jpeg("menu/lighting3_.jpg",0x8c,uVar2);
        value = 2;
        goto LAB_ram_000142ea;
      }
      pcVar1 = "menu/lighting3.jpg";
    }
  }
  else {
    if (param_1 != 0) {
      resource_draw_jpeg("menu/lighting1_.jpg",0x8c,uVar2);
      value = 0;
      goto LAB_ram_000142ea;
    }
    pcVar1 = "menu/lighting1.jpg";
  }
  resource_draw_jpeg(pcVar1,0x8c,uVar2);
  return;
}

