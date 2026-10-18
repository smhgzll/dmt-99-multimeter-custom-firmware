/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00010e86; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00010e86; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_00010e86(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  
  gp = &__global_pointer;
  if (param_2 == 1) {
    iVar1 = 0x1b;
LAB_ram_00010f2c:
    if (param_1 == 0) {
LAB_ram_00010f2e:
      iVar2 = settings_language_access(0,0);
      if (iVar2 == 0) {
        pcVar4 = "menu/min_c.jpg";
      }
      else {
        pcVar4 = "menu/min.jpg";
      }
LAB_ram_00010f46:
      resource_draw_jpeg(pcVar4,0x8d,iVar1);
      if ((param_2 - 1U & 0xff) < 2) {
        uVar5 = 0x11a;
        pcVar4 = "menu/v.jpg";
      }
      else if ((param_2 - 3U & 0xff) < 2) {
        uVar5 = 0x11a;
        pcVar4 = "menu/a.jpg";
      }
      else {
        uVar5 = 0x116;
        pcVar4 = "menu/c.jpg";
      }
      resource_draw_jpeg(pcVar4,uVar5,iVar1 + 3);
      FUN_ram_00008050(0xc1,iVar1,0x12d,iVar1,0x2105);
      FUN_ram_00008050(0xc1,iVar1 + 0x15,0x12d,iVar1 + 0x15,0x2105);
      pcVar4 = "menu/max_min_right.jpg";
      goto LAB_ram_00010fa6;
    }
LAB_ram_00010fe2:
    iVar2 = settings_language_access(0,0);
    if (iVar2 == 0) {
      pcVar4 = "menu/min_c_.jpg";
    }
    else {
      pcVar4 = "menu/min_.jpg";
    }
  }
  else {
    if (param_2 == 2) {
      iVar2 = 0x36;
      if (param_1 == 0) {
LAB_ram_00011064:
        iVar3 = settings_language_access(0,0);
        iVar1 = iVar2;
        if (iVar3 == 0) {
          pcVar4 = "menu/max_c.jpg";
        }
        else {
          pcVar4 = "menu/max.jpg";
        }
        goto LAB_ram_00010f46;
      }
    }
    else {
      if (param_2 == 3) {
        iVar1 = 0x68;
        goto LAB_ram_00010f2c;
      }
      if (param_2 == 4) {
        iVar2 = 0x84;
LAB_ram_00011062:
        if (param_1 == 0) goto LAB_ram_00011064;
      }
      else {
        if (param_2 != 5) {
          if (param_2 != 6) {
            gp = &__global_pointer;
            return 0;
          }
          iVar2 = 0xd2;
          goto LAB_ram_00011062;
        }
        iVar1 = 0xb6;
        iVar2 = 0xb6;
        if (param_1 == 0) goto LAB_ram_00010f2e;
      }
      iVar1 = iVar2;
      if ((param_2 - 3U & 0xfd) == 0) goto LAB_ram_00010fe2;
    }
    iVar3 = settings_language_access(0,0);
    iVar1 = iVar2;
    if (iVar3 == 0) {
      pcVar4 = "menu/max_c_.jpg";
    }
    else {
      pcVar4 = "menu/max_.jpg";
    }
  }
  resource_draw_jpeg(pcVar4,0x8d,iVar1);
  if ((param_2 - 1U & 0xff) < 2) {
    uVar5 = 0x11a;
    pcVar4 = "menu/v_.jpg";
  }
  else if ((param_2 - 3U & 0xff) < 2) {
    uVar5 = 0x11a;
    pcVar4 = "menu/a_.jpg";
  }
  else {
    uVar5 = 0x116;
    pcVar4 = "menu/c_.jpg";
  }
  resource_draw_jpeg(pcVar4,uVar5,iVar1 + 3);
  FUN_ram_00008050(0xc1,iVar1,0x12d,iVar1,0xfd40);
  FUN_ram_00008050(0xc1,iVar1 + 0x15,0x12d,iVar1 + 0x15,0xfd40);
  pcVar4 = "menu/max_min_right_.jpg";
LAB_ram_00010fa6:
  uVar5 = resource_draw_jpeg(pcVar4,0x12d,iVar1);
  return uVar5;
}

