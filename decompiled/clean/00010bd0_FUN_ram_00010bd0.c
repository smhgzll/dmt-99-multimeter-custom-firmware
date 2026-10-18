/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00010bd0; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00010bd0; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */

void FUN_ram_00010bd0(int param_1)

{
  short sVar1;
  int iVar2;
  undefined *puVar3;
  char *pcVar4;
  short sVar5;
  int iVar6;
  short sVar7;
  int iVar8;
  undefined4 uVar9;
  uint uVar10;
  char *pcVar11;
  
  gp = &__global_pointer;
  if (application_view == '\x03') {
    iVar2 = 0x82;
    sVar1 = 0x23;
  }
  else {
    iVar2 = 0x5a;
    sVar1 = 0x1e;
  }
  uVar10 = 0;
  do {
    pcVar11 = (char *)(param_1 + uVar10);
    if (*pcVar11 == 0) {
      if ((measurement_mode == '\x12') ||
         (((measurement_mode == '\x16' || (measurement_mode == '\x11')) &&
          (measurement_unit_code == '\b')))) {
        display_fill_rectangle(sVar1 + -5,iVar2,sVar1 + 3,iVar2 + 0x33,0);
      }
      return;
    }
    switch((int)*pcVar11 - 0x2dU & 0xff) {
    case 0:
      uVar9 = 0xfd40;
LAB_ram_00010da6:
      sVar7 = sVar1 + 0x1e;
      sVar5 = sVar1 + 0xd;
      iVar6 = iVar2 + 0x14;
      iVar8 = iVar2 + 0x19;
      goto LAB_ram_00010e3e;
    case 1:
      sVar5 = sVar1 + -5;
      pcVar4 = "num/radix.jpg";
      goto LAB_ram_00010d92;
    default:
      if (uVar10 == 0) {
        uVar9 = 0;
        goto LAB_ram_00010da6;
      }
      sVar7 = sVar1 + 0x26;
      uVar9 = 0;
      iVar6 = iVar2;
      iVar8 = iVar2 + 0x33;
      sVar5 = sVar1;
LAB_ram_00010e3e:
      display_fill_rectangle(sVar5,iVar6,sVar7,iVar8,uVar9);
      goto LAB_ram_00010c34;
    case 3:
    case 0x22:
      puVar3 = &DAT_ram_000254c8;
      break;
    case 4:
      puVar3 = &DAT_ram_000255d0;
      break;
    case 5:
      puVar3 = &DAT_ram_000256d8;
      break;
    case 6:
      puVar3 = &DAT_ram_000257e0;
      break;
    case 7:
      puVar3 = &DAT_ram_000258e8;
      break;
    case 8:
      puVar3 = &DAT_ram_000259f0;
      break;
    case 9:
      puVar3 = &DAT_ram_00025af8;
      break;
    case 10:
      puVar3 = &DAT_ram_00025c00;
      break;
    case 0xb:
      puVar3 = &DAT_ram_00025d08;
      break;
    case 0xc:
      puVar3 = &DAT_ram_00025e10;
      break;
    case 0x14:
      puVar3 = &DAT_ram_00025f18;
      break;
    case 0x18:
      pcVar4 = "num/e.jpg";
      sVar5 = sVar1;
LAB_ram_00010d92:
      resource_draw_jpeg(pcVar4,sVar5,iVar2);
      goto LAB_ram_00010c34;
    case 0x1f:
      puVar3 = &DAT_ram_00026020;
      break;
    case 0x27:
      puVar3 = &DAT_ram_00026128;
      break;
    case 0x28:
      puVar3 = &DAT_ram_00026230;
    }
    FUN_ram_0001a360(puVar3,sVar1,iVar2,0xfd40,0,0x32);
LAB_ram_00010c34:
    if ((*pcVar11 == '.') ||
       (((pcVar11[-1] != '.' && (uVar10 != 0)) &&
        (display_fill_rectangle(sVar1 + -5,iVar2,sVar1,iVar2 + 0x33,0), *pcVar11 == '.')))) {
      sVar1 = sVar1 + 8;
    }
    else {
      sVar1 = sVar1 + 0x2b;
    }
    uVar10 = uVar10 + 1 & 0xff;
  } while( true );
}

