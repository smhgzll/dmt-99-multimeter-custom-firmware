/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000c11a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c11a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_0000c11a(int param_1,int param_2)

{
  undefined4 uVar1;
  char *pcVar2;
  
  gp = &__global_pointer;
  if (DAT_ram_200001db == measurement_unit_code) {
    uVar1 = 0;
  }
  else {
    DAT_ram_200001db = measurement_unit_code;
    uVar1 = display_fill_rectangle
                      (param_1,param_2,param_1 + 0x3cU & 0xffff,param_2 + 0x28U & 0xffff,0);
    switch(measurement_unit_code + -6) {
    case '\0':
      pcVar2 = "units/v.jpg";
      break;
    case '\x01':
      pcVar2 = "units/mv.jpg";
      break;
    case '\x02':
      pcVar2 = "units/ua.jpg";
      break;
    case '\x03':
      pcVar2 = "units/ma.jpg";
      break;
    case '\x04':
      pcVar2 = "units/a.jpg";
      break;
    case '\x05':
      pcVar2 = "units/r.jpg";
      break;
    case '\x06':
      pcVar2 = "units/kr.jpg";
      break;
    case '\a':
      pcVar2 = "units/mr.jpg";
      break;
    default:
      goto switchD_ram_00014448_default;
    case '\t':
      pcVar2 = "units/nf.jpg";
      break;
    case '\n':
      pcVar2 = "units/uf.jpg";
      break;
    case '\v':
      pcVar2 = "units/mf.jpg";
      break;
    case '\f':
      pcVar2 = "units/hz.jpg";
      break;
    case '\r':
      pcVar2 = "units/khz.jpg";
      break;
    case '\x0e':
      pcVar2 = "units/mhz.jpg";
      break;
    case '\x0f':
      pcVar2 = "units/c.jpg";
    }
    uVar1 = resource_draw_jpeg(pcVar2,param_1,param_2);
  }
switchD_ram_00014448_default:
  return uVar1;
}

