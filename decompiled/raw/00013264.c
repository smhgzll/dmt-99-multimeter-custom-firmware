/* Original address: 0x00013264; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00013264(void)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 uVar7;
  char *pcVar8;
  uint uVar9;
  int iVar10;
  undefined8 uVar11;
  
  gp = &__global_pointer;
  uVar4 = measurement_get_value_bits();
  iVar10 = 0;
  if (display_layout == '\0') {
    iVar10 = 5;
  }
  switch(measurement_mode - 0xc) {
  case 0:
  case 1:
    uVar3 = DAT_ram_20000032 & 0x7fff;
    uVar1 = DAT_ram_20000034;
    if ((short)DAT_ram_20000032 < 0) {
      uVar3 = -uVar3;
    }
    goto LAB_ram_000132ba;
  default:
    uVar2 = 0;
    uVar3 = 0;
    break;
  case 4:
  case 9:
    uVar4 = FUN_ram_0001db48(uVar4,0x42c80000);
    uVar3 = DAT_ram_20000036 & 0x7fff;
    uVar1 = DAT_ram_20000038;
    if ((short)DAT_ram_20000036 < 0) {
      uVar3 = -uVar3;
    }
LAB_ram_000132ba:
    uVar2 = uVar1 & 0x7fff;
    if ((short)uVar1 < 0) {
      uVar2 = -uVar2;
    }
    break;
  case 6:
    uVar3 = DAT_ram_2000003a & 0x7fff;
    if ((short)DAT_ram_2000003a < 0) {
      uVar3 = -uVar3;
    }
    uVar2 = DAT_ram_2000003c & 0x7fff;
    if ((short)DAT_ram_2000003c < 0) {
      uVar2 = -uVar2;
    }
    uVar9 = settings_temperature_access(0,0);
    if (uVar9 == 1) {
      uVar11 = FUN_ram_0001d072(uVar3);
      uVar11 = FUN_ram_0001c3b8((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0xcccccccd,0x3ffccccc);
      FUN_ram_0001b61e((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0,0x40400000);
      uVar3 = FUN_ram_0001cfae();
      uVar11 = FUN_ram_0001d072(uVar2);
      uVar11 = FUN_ram_0001c3b8((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0xcccccccd,0x3ffccccc);
      FUN_ram_0001b61e((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0,0x40400000);
      uVar2 = FUN_ram_0001cfae();
      uVar11 = FUN_ram_0001e2e2(uVar4);
      uVar11 = FUN_ram_0001c3b8((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0xcccccccd,0x3ffccccc);
      FUN_ram_0001b61e((int)uVar11,(int)((ulonglong)uVar11 >> 0x20),0,0x40400000);
      uVar4 = FUN_ram_0001e38e();
    }
  }
  if (9 < (measurement_mode - 0xc & 0xff)) {
    return;
  }
  if ((0x253U >> (measurement_mode - 0xc & 0x1f) & 1) == 0) {
    return;
  }
  uVar5 = FUN_ram_0001e21c(uVar3);
  iVar6 = FUN_ram_0001dabc(uVar5,uVar4);
  if (iVar6 < 0) {
    uVar7 = FUN_ram_0001e21c(uVar2);
    iVar6 = FUN_ram_0001da36(uVar7,uVar4);
    if (iVar6 < 1) goto LAB_ram_0001344a;
    if (DAT_ram_200001db != '\0') {
      buzzer_enable(0);
      DAT_ram_200001db = 0;
      resource_draw_jpeg("mode3_medi1.jpg",0x13,iVar10 + 0x14);
      resource_draw_jpeg("mode3_medi2.jpg",0x78,iVar10 + 0x14);
      pcVar8 = "mode3_medi3.jpg";
      goto LAB_ram_00013350;
    }
    iVar6 = FUN_ram_0001da36(uVar5,uVar4);
    if (0 < iVar6) goto LAB_ram_00013460;
  }
  else {
LAB_ram_0001344a:
    iVar6 = FUN_ram_0001da36(uVar5,uVar4);
    if ((0 < iVar6) && (DAT_ram_200001db != '\x01')) {
LAB_ram_00013460:
      buzzer_enable(1);
      DAT_ram_200001db = 1;
      resource_draw_jpeg("mode3_min1.jpg",0x13,iVar10 + 0x14);
      resource_draw_jpeg("mode3_max2.jpg",0x78,iVar10 + 0x14);
      pcVar8 = "mode3_min3.jpg";
      goto LAB_ram_00013350;
    }
  }
  uVar5 = FUN_ram_0001e21c(uVar2);
  iVar6 = FUN_ram_0001dabc(uVar5,uVar4);
  if (-1 < iVar6) {
    return;
  }
  if (DAT_ram_200001db == '\x02') {
    return;
  }
  buzzer_enable(1);
  DAT_ram_200001db = 2;
  resource_draw_jpeg("mode3_max1.jpg",0x13,iVar10 + 0x14);
  resource_draw_jpeg("mode3_max2.jpg",0x78,iVar10 + 0x14);
  pcVar8 = "mode3_max3.jpg";
LAB_ram_00013350:
  resource_draw_jpeg(pcVar8,199,iVar10 + 0x14);
  return;
}

