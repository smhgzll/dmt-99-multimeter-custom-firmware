/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00012b00; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00012b00; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00012b00(char param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined1 auStack_18 [8];
  
  gp = &__global_pointer;
  switch(param_1 + -6) {
  case '\0':
    uVar4 = 0x20;
    uVar3 = 0x20;
    uVar2 = 0x56;
    break;
  case '\x01':
    uVar4 = 0x20;
    uVar3 = 0x56;
    goto LAB_ram_00012b6c;
  case '\x02':
    uVar3 = 0x41;
    goto LAB_ram_00012b98;
  case '\x03':
    uVar4 = 0x20;
    uVar3 = 0x41;
    goto LAB_ram_00012b6c;
  case '\x04':
    uVar4 = 0x20;
    uVar3 = 0x20;
    uVar2 = 0x41;
    break;
  case '\x05':
    uVar4 = 0x20;
    uVar3 = 0x20;
    uVar2 = 0x58;
    break;
  case '\x06':
    uVar4 = 0x20;
    uVar3 = 0x58;
    goto LAB_ram_00012bb8;
  case '\a':
    uVar4 = 0x20;
    uVar3 = 0x58;
    uVar2 = 0x4d;
    break;
  case '\b':
    goto switchD_ram_00014448_default;
  case '\t':
  case '\v':
    uVar4 = 0x20;
    uVar3 = 0x46;
    uVar2 = 0x6e;
    break;
  case '\n':
    uVar3 = 0x46;
LAB_ram_00012b98:
    uVar4 = 0x20;
    uVar2 = 0x59;
    break;
  case '\f':
    uVar4 = 0x20;
    uVar3 = 0x7a;
    uVar2 = 0x48;
    break;
  case '\r':
    uVar4 = 0x7a;
    uVar3 = 0x48;
LAB_ram_00012bb8:
    uVar2 = 0x6b;
    break;
  case '\x0e':
    uVar4 = 0x7a;
    uVar3 = 0x48;
LAB_ram_00012b6c:
    uVar2 = 0x6d;
    break;
  case '\x0f':
    uVar1 = settings_temperature_access(0,0);
    uVar2 = 0x5a;
    uVar3 = 0x20;
    uVar4 = 0x20;
    if (uVar1 != 0) {
      uVar2 = 0x4a;
      uVar3 = 0x20;
      uVar4 = 0x20;
    }
    break;
  default:
    return;
  }
  FUN_ram_0000969e(auStack_18,&DAT_ram_00023788,uVar2,uVar3,uVar4,0);
  FUN_ram_000085cc(0x120,param_2 + 2,0x3c,0x14,0x10,param_3,auStack_18);
switchD_ram_00014448_default:
  return;
}

