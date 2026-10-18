/* Original address: 0x00008e46; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00008e46(uint param_1,undefined4 param_2,char *param_3,undefined4 param_4,
                     undefined4 param_5,int param_6,undefined4 param_7)

{
  gp = &__global_pointer;
  do {
    if (*param_3 == '\0') {
      return;
    }
    if (param_6 == 0xc) {
      FUN_ram_00008a5a(param_1,param_2,param_3,param_4,param_5,0xc,param_7);
    }
    else if (param_6 == 0x10) {
      FUN_ram_00008b5c(param_1,param_2,param_3,param_4,param_5,0x10,param_7);
    }
    else if (param_6 == 0x18) {
      FUN_ram_00008c64(param_1,param_2,param_3,param_4,param_5,0x18,param_7);
    }
    else {
      if (param_6 != 0x20) {
        gp = &__global_pointer;
        return;
      }
      FUN_ram_00008d6c(param_1,param_2,param_3,param_4,param_5,0x20,param_7);
    }
    param_3 = param_3 + 2;
    param_1 = param_1 + param_6 & 0xffff;
  } while( true );
}

