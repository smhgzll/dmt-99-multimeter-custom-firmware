/* Semantic review: high. Loads packed settings and applies theme language layout and auto-off.
 * Address 0x0000c248; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c248; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Loads packed settings and applies theme language layout and auto-off */

void settings_load_and_apply(void)

{
  uint uVar1;
  int iVar2;
  char *pcVar3;
  
  gp = &__global_pointer;
  if (DAT_ram_20000224 == '\0') {
    settings_load_internal_flash();
  }
  if (DAT_ram_20000020 != 0) {
    normal_operation = 1;
    settings_restore_defaults();
    FUN_ram_00008016();
    FUN_ram_00015168();
    DAT_ram_20000020 = 0;
  }
  uVar1 = settings_theme_access(0,1);
  if ((uVar1 - 1 & 0xff) < 3) {
    application_view = (undefined1)uVar1;
  }
  else {
    application_view = 1;
    settings_theme_access(1,1);
  }
  iVar2 = settings_language_access(0,0);
  if (iVar2 == 0) {
    pcVar3 = "menu/chinese/";
  }
  else {
    pcVar3 = "menu/english/";
  }
  FUN_ram_0000969e(&resource_language_prefix,&DAT_ram_00024744,pcVar3);
  uVar1 = settings_layout_access(0,0);
  display_layout = uVar1 == 1;
  uVar1 = settings_auto_off_access(0,0);
  if (uVar1 == 1) {
    auto_off_seconds = (undefined *)0x12c;
  }
  else if ((char)uVar1 == '\0') {
    auto_off_seconds = (undefined *)0x0;
  }
  else if (uVar1 == 2) {
    auto_off_seconds = &mdbase;
  }
  else if (uVar1 == 3) {
    auto_off_seconds = (undefined *)0x708;
  }
  return;
}

