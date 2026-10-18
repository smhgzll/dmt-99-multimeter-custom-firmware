/* Original address: 0x00014408; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_00014408(char param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  gp = &__global_pointer;
  if (display_layout == '\0') {
    uVar4 = 0xe4;
    uVar3 = 0x13f;
    uVar2 = 0xb;
    uVar1 = 0x13d;
  }
  else {
    uVar4 = 0xea;
    uVar3 = 0x13d;
    uVar2 = 5;
    uVar1 = 0x13b;
  }
  display_fill_rectangle(uVar1,uVar2,uVar3,uVar4,0x4228);
  switch(param_1 + -1) {
  case '\0':
    if (display_layout != '\0') {
      uVar4 = 0x57;
      uVar3 = 6;
      break;
    }
    uVar4 = 0x48;
    uVar3 = 0xc;
LAB_ram_00014470:
    uVar5 = 0x1c9f;
    uVar2 = 0x13e;
    uVar1 = 0x13e;
    goto LAB_ram_00014474;
  case '\x01':
    if (display_layout == '\0') {
      uVar4 = 0x67;
      uVar3 = 0x2b;
      goto LAB_ram_00014470;
    }
    uVar4 = 0x73;
    uVar3 = 0x22;
    break;
  case '\x02':
    if (display_layout == '\0') {
      uVar4 = 0x86;
      uVar3 = 0x4a;
      goto LAB_ram_00014470;
    }
    uVar4 = 0x8f;
    uVar3 = 0x3e;
    break;
  case '\x03':
    if (display_layout == '\0') {
      uVar4 = 0xa5;
      uVar3 = 0x69;
      goto LAB_ram_00014470;
    }
    uVar4 = 0xab;
    uVar3 = 0x5a;
    break;
  case '\x04':
    if (display_layout == '\0') {
      uVar4 = 0xc4;
      uVar3 = 0x88;
      goto LAB_ram_00014470;
    }
    uVar4 = 199;
    uVar3 = 0x76;
    break;
  case '\x05':
    if (display_layout == '\0') {
      uVar4 = 0xe3;
      uVar3 = 0xa7;
      goto LAB_ram_00014470;
    }
    uVar4 = 0xe6;
    uVar3 = 0x95;
    break;
  default:
    goto switchD_ram_00014448_default;
  }
  uVar5 = 0xfd40;
  uVar2 = 0x13c;
  uVar1 = 0x13c;
LAB_ram_00014474:
  FUN_ram_00008050(uVar1,uVar3,uVar2,uVar4,uVar5);
switchD_ram_00014448_default:
  return;
}

