/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000109e4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000109e4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_000109e4(int param_1)

{
  short sVar1;
  int iVar2;
  char *pcVar3;
  short sVar4;
  uint uVar5;
  char *pcVar6;
  
  gp = &__global_pointer;
  if (application_view == '\x03') {
    iVar2 = 0x78;
    sVar1 = 0x2e;
  }
  else {
    iVar2 = 0x5a;
    sVar1 = 0x2b;
  }
  uVar5 = 0;
  do {
    pcVar6 = (char *)(param_1 + uVar5);
    if (*pcVar6 == 0) {
      if ((measurement_mode == '\x12') ||
         (((measurement_mode == '\x16' || (measurement_mode == '\x11')) &&
          (measurement_unit_code == '\b')))) {
        display_fill_rectangle(sVar1 + -7,iVar2,sVar1 + -1,iVar2 + 0x33,0);
      }
      wch_restore_12();
      return;
    }
    sVar4 = sVar1;
    switch((int)*pcVar6 - 0x2dU & 0xff) {
    case 0:
      pcVar3 = "num/minus.jpg";
      break;
    case 1:
      pcVar3 = "num/radix.jpg";
      sVar4 = sVar1 + -7;
      break;
    default:
      display_fill_rectangle(sVar1,iVar2,sVar1 + 0x21,iVar2 + 0x33,0);
      goto LAB_ram_00010a44;
    case 3:
    case 0x22:
      pcVar3 = "num/0.jpg";
      break;
    case 4:
      pcVar3 = "num/1.jpg";
      break;
    case 5:
      pcVar3 = "num/2.jpg";
      break;
    case 6:
      pcVar3 = "num/3.jpg";
      break;
    case 7:
      pcVar3 = "num/4.jpg";
      break;
    case 8:
      pcVar3 = "num/5.jpg";
      break;
    case 9:
      pcVar3 = "num/6.jpg";
      break;
    case 10:
      pcVar3 = "num/7.jpg";
      break;
    case 0xb:
      pcVar3 = "num/8.jpg";
      break;
    case 0xc:
      pcVar3 = "num/9.jpg";
      break;
    case 0x14:
      pcVar3 = "num/a.jpg";
      break;
    case 0x18:
      pcVar3 = "num/e.jpg";
      break;
    case 0x1f:
      pcVar3 = "num/l.jpg";
      break;
    case 0x27:
      pcVar3 = "num/t.jpg";
      break;
    case 0x28:
      pcVar3 = "num/u.jpg";
    }
    resource_draw_jpeg(pcVar3,sVar4,iVar2);
LAB_ram_00010a44:
    if ((*pcVar6 == '.') ||
       (((pcVar6[-1] != '.' && (uVar5 != 0)) &&
        (display_fill_rectangle(sVar1 + -7,iVar2,sVar1 + -1,iVar2 + 0x33,0), *pcVar6 == '.')))) {
      sVar1 = sVar1 + 7;
    }
    else {
      sVar1 = sVar1 + 0x28;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while( true );
}

