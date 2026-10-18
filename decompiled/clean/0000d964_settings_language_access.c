/* Semantic review: high. Packed word low three bits and language resource path.
 * Address 0x0000d964; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d964; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Packed word low three bits and language resource path */

uint settings_language_access(uint param_1,undefined4 param_2)

{
  uint uVar1;
  char *pcVar2;
  
  gp = &__global_pointer;
  if (param_1 != 0) {
    settings_word = settings_word & 0xfff8 | (ushort)param_2;
    switch(param_2) {
    case 0:
      pcVar2 = "menu/chinese/";
      break;
    case 1:
      pcVar2 = "menu/english/";
      break;
    case 2:
      pcVar2 = "menu/german/";
      break;
    case 3:
      pcVar2 = "menu/japanese/";
      break;
    case 4:
      pcVar2 = "menu/portuguese/";
      break;
    case 5:
      pcVar2 = "menu/russian/";
      break;
    default:
      return param_1;
    }
    uVar1 = FUN_ram_0000969e(&resource_language_prefix,&DAT_ram_00024744,pcVar2);
    return uVar1;
  }
  return settings_word & 7;
}

