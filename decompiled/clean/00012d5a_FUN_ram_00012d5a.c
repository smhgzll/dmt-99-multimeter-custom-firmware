/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00012d5a; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00012d5a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_6 replaced with injection: wch_save_6 */

void FUN_ram_00012d5a(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  uint uVar6;
  undefined *puVar7;
  undefined8 uVar8;
  undefined1 auStack_28 [8];
  
  gp = &__global_pointer;
  if (display_layout == '\0') {
    resource_draw_jpeg("motif2/mode3_value.jpg",0,0xcf);
    iVar3 = settings_language_access(0,0);
    if (iVar3 == 0) {
      FUN_ram_00008e46(0x88,0xd3,&DAT_ram_00025208,0xffff,0xffff,0x18);
    }
    else {
      FUN_ram_00008588(0x7d,0xd6,"Threshold",0xffff,0xffff,0x10,1);
    }
  }
  else {
    display_fill_rectangle(0x1e,0xc3,0x62,0xda,0);
    display_fill_rectangle(0xf0,0xc3,0x129,0xda,0);
  }
  if ((param_1 - 0xcU & 0xff) < 2) {
    uVar6 = DAT_ram_20000032 & 0x7fff;
    if ((short)DAT_ram_20000032 < 0) {
      uVar6 = -uVar6;
    }
    uVar1 = DAT_ram_20000034 & 0x7fff;
    if ((short)DAT_ram_20000034 < 0) {
      uVar1 = -uVar1;
    }
    FUN_ram_0000969e(auStack_28,&PTR_DAT_ram_00006425_ram_0002521c,uVar6);
    if (display_layout == '\0') {
      FUN_ram_00008588(0x19,0xd2,auStack_28,0xffff,0xffff,0x18,1);
      FUN_ram_00008588(0x50,0xd7,&DAT_ram_00025220,0xffff,0xffff,0x10,1);
    }
    else {
      FUN_ram_000085cc(0x19,0xc2,0x50,0x28,0x18,0xffff,auStack_28);
      resource_draw_jpeg("units/v.jpg",0x50,0xc9);
      resource_draw_jpeg("units/v.jpg",0x116,0xc9);
    }
    FUN_ram_0000969e(auStack_28,&PTR_DAT_ram_00006425_ram_0002521c,uVar1);
    if (display_layout != '\0') {
LAB_ram_00012f0a:
      FUN_ram_000085cc(0xe1,0xc2,0x50,0x28,0x18,0xffff,auStack_28);
      gp = &__global_pointer;
      return;
    }
    FUN_ram_00008588(0xeb,0xd2,auStack_28,0xffff,0xffff,0x18,1);
    puVar7 = &DAT_ram_00025220;
    uVar2 = 0x118;
  }
  else {
    if ((param_1 != 0x10) && (param_1 != 0x15)) {
      if (param_1 != 0x12) {
        gp = &__global_pointer;
        return;
      }
      uVar6 = DAT_ram_2000003a & 0x7fff;
      if ((short)DAT_ram_2000003a < 0) {
        uVar6 = -uVar6;
      }
      uVar1 = DAT_ram_2000003c & 0x7fff;
      if ((short)DAT_ram_2000003c < 0) {
        uVar1 = -uVar1;
      }
      uVar4 = settings_temperature_access(0,0);
      if (uVar4 == 1) {
        uVar8 = FUN_ram_0001d072(uVar6);
        uVar8 = FUN_ram_0001c3b8((int)uVar8,(int)((ulonglong)uVar8 >> 0x20),0xcccccccd,0x3ffccccc);
        FUN_ram_0001b61e((int)uVar8,(int)((ulonglong)uVar8 >> 0x20),0,0x40400000);
        uVar6 = FUN_ram_0001cfae();
        uVar8 = FUN_ram_0001d072(uVar1);
        uVar8 = FUN_ram_0001c3b8((int)uVar8,(int)((ulonglong)uVar8 >> 0x20),0xcccccccd,0x3ffccccc);
        FUN_ram_0001b61e((int)uVar8,(int)((ulonglong)uVar8 >> 0x20),0,0x40400000);
        uVar1 = FUN_ram_0001cfae();
      }
      FUN_ram_0000969e(auStack_28,&PTR_DAT_ram_00006425_ram_0002521c,uVar6);
      if (display_layout == '\0') {
        FUN_ram_00008588(0x19,0xd2,auStack_28,0xffff,0xffff,0x18,1);
      }
      else {
        FUN_ram_000085cc(0x19,0xc2,0x50,0x28,0x18,0xffff,auStack_28);
      }
      FUN_ram_0000969e(auStack_28,&PTR_DAT_ram_00006425_ram_0002521c,uVar1);
      if (display_layout == '\0') {
        FUN_ram_00008588(0xeb,0xd2,auStack_28,0xffff,0xffff,0x18,1);
      }
      else {
        FUN_ram_000085cc(0xe1,0xc2,0x50,0x28,0x18,0xffff,auStack_28);
      }
      uVar6 = settings_temperature_access(0,0);
      if (uVar6 == 0) {
        if (display_layout != '\0') {
          resource_draw_jpeg("units/c.jpg",0x50,0xc9);
          pcVar5 = "units/c.jpg";
LAB_ram_0001320a:
          resource_draw_jpeg(pcVar5,0x116,0xc9);
          gp = &__global_pointer;
          return;
        }
        FUN_ram_00008e46(0x50,0xd7,&DAT_ram_00025230,0xffff,0xffff,0x10,1);
        puVar7 = &DAT_ram_00025230;
      }
      else {
        if (display_layout != '\0') {
          resource_draw_jpeg("units/f.jpg",0x50,0xc9);
          pcVar5 = "units/f.jpg";
          goto LAB_ram_0001320a;
        }
        FUN_ram_00008e46(0x50,0xd7,&DAT_ram_00025234,0xffff,0xffff,0x10,1);
        puVar7 = &DAT_ram_00025234;
      }
      FUN_ram_00008e46(0x118,0xd7,puVar7,0xffff,0xffff,0x10,1);
      gp = &__global_pointer;
      return;
    }
    uVar6 = DAT_ram_20000036 & 0x7fff;
    if ((short)DAT_ram_20000036 < 0) {
      uVar6 = -uVar6;
    }
    uVar1 = DAT_ram_20000038 & 0x7fff;
    if ((short)DAT_ram_20000038 < 0) {
      uVar1 = -uVar1;
    }
    uVar2 = FUN_ram_0001e21c(uVar6);
    FUN_ram_0001d728(uVar2,0x42c80000);
    uVar8 = FUN_ram_0001e2e2();
    FUN_ram_0000969e(auStack_28,"%0.2f",(int)uVar8,(int)((ulonglong)uVar8 >> 0x20));
    if (display_layout == '\0') {
      FUN_ram_00008588(0x19,0xd2,auStack_28,0xffff,0xffff,0x18,1);
      FUN_ram_00008588(0x5a,0xd7,&DAT_ram_0002522c,0xffff,0xffff,0x10,1);
    }
    else {
      FUN_ram_000085cc(0x19,0xc2,0x50,0x28,0x18,0xffff,auStack_28);
      resource_draw_jpeg("units/a.jpg",0x50,0xc9);
      resource_draw_jpeg("units/a.jpg",0x116,0xc9);
    }
    uVar2 = FUN_ram_0001e21c(uVar1);
    FUN_ram_0001d728(uVar2,0x42c80000);
    uVar8 = FUN_ram_0001e2e2();
    FUN_ram_0000969e(auStack_28,"%0.2f",(int)uVar8,(int)((ulonglong)uVar8 >> 0x20));
    if (display_layout != '\0') goto LAB_ram_00012f0a;
    FUN_ram_00008588(0xeb,0xd2,auStack_28,0xffff,0xffff,0x18,1);
    puVar7 = &DAT_ram_0002522c;
    uVar2 = 0x120;
  }
  FUN_ram_00008588(uVar2,0xd7,puVar7,0xffff,0xffff,0x10,1);
  return;
}

