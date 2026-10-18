/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x000164d4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x000164d4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

undefined4 FUN_ram_000164d4(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  gp = &__global_pointer;
  if ((button_event != '\x01') && (button_event != '\x02')) {
    return 0;
  }
  if (button_event == '\x01') {
    switch(DAT_ram_200001d8 + -1) {
    case '\0':
      uVar3 = 1;
      if (display_layout == '\0') {
LAB_ram_000167d8:
        FUN_ram_00018ad2(0,2,uVar3);
      }
      else {
        iVar1 = settings_language_access(0,0);
        uVar3 = 1;
        if (iVar1 == 0) {
LAB_ram_000167f0:
          FUN_ram_00019366(0,2,uVar3);
        }
      }
LAB_ram_00016534:
      DAT_ram_200001d8 = '\x05';
      break;
    case '\x01':
switchD_ram_0001651a_caseD_1:
      DAT_ram_200001d8 = '\x03';
      break;
    case '\x02':
switchD_ram_0001651a_caseD_2:
      DAT_ram_200001d8 = '\x04';
      break;
    case '\x03':
switchD_ram_0001651a_caseD_3:
      DAT_ram_200001d8 = '\x02';
      break;
    case '\x04':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0x14,0x140,0xdc,0);
        uVar3 = 2;
LAB_ram_00016554:
        FUN_ram_00018ad2(0,3,uVar3);
        iVar1 = settings_language_access(0,0);
        if (iVar1 != 0) {
          FUN_ram_00008588(0xa9,0x1a,&DAT_ram_00025158,0xfd40,0xfd40,0x10,1);
          FUN_ram_00008588(0xee,0x1a,"Measurements",0xfd40,0xfd40,0x10,1);
          goto LAB_ram_0001686c;
        }
        FUN_ram_00008e46(0xa9,0x1a,&DAT_ram_00025148);
        uVar4 = 0x10;
        uVar2 = 0x1a;
        uVar3 = 0xf5;
      }
      else {
        iVar1 = settings_language_access(0,0);
        if (iVar1 != 0) goto LAB_ram_0001686c;
        display_fill_rectangle(0x83,0,0x140,0xf0,0);
        FUN_ram_00019366(0,3,2);
LAB_ram_000165ce:
        FUN_ram_00008e46(0x9f,0xc,&DAT_ram_00025148,0xfd40,0xfd40,0x18,1);
        uVar4 = 0x18;
        uVar2 = 0xc;
        uVar3 = 0xee;
      }
      FUN_ram_00008e46(uVar3,uVar2,&DAT_ram_00025150,0xfd40,0xfd40,uVar4,1);
LAB_ram_0001686c:
      DAT_ram_200001d8 = '\f';
      break;
    case '\x05':
switchD_ram_0001651a_caseD_5:
      DAT_ram_200001d8 = '\a';
      break;
    case '\x06':
switchD_ram_0001651a_caseD_6:
      DAT_ram_200001d8 = '\b';
      break;
    case '\a':
switchD_ram_0001651a_caseD_7:
      DAT_ram_200001d8 = '\t';
      break;
    case '\b':
switchD_ram_0001651a_caseD_8:
      DAT_ram_200001d8 = '\n';
      break;
    case '\t':
      DAT_ram_200001d8 = '\v';
      break;
    case '\v':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0xf,300,0x14,0);
        display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
        display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
        uVar3 = 3;
        goto LAB_ram_00016658;
      }
      iVar1 = settings_language_access(0,0);
      uVar3 = 3;
      if (iVar1 == 0) {
LAB_ram_0001696c:
        FUN_ram_00019366(0,4,uVar3);
      }
LAB_ram_00016676:
      display_fill_rectangle(0x83,0,0x140,0xf0,0);
LAB_ram_00016660:
      FUN_ram_000153d0();
      DAT_ram_200001d8 = '\r';
      break;
    case '\f':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0xf,300,0x14,0);
        display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
        display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
        uVar3 = 4;
LAB_ram_000166d0:
        FUN_ram_00018ad2(0,5,uVar3);
      }
      else {
        iVar1 = settings_language_access(0,0);
        uVar3 = 4;
        if (iVar1 == 0) {
LAB_ram_000169c8:
          FUN_ram_00019366(0,5,uVar3);
        }
      }
LAB_ram_000166e8:
      DAT_ram_200001d8 = '\x1d';
      break;
    case '\r':
switchD_ram_0001651a_caseD_d:
      DAT_ram_200001d8 = '\x0f';
      break;
    case '\x0e':
      FUN_ram_00015e9e();
switchD_ram_00016762_caseD_16:
      DAT_ram_200001d8 = '\x16';
      break;
    case '\x13':
switchD_ram_0001651a_caseD_13:
      DAT_ram_200001d8 = '\x15';
      break;
    case '\x14':
switchD_ram_0001651a_caseD_14:
      DAT_ram_200001d8 = '\x0e';
      break;
    case '\x15':
switchD_ram_0001651a_caseD_15:
      DAT_ram_200001d8 = '\x17';
      break;
    case '\x16':
switchD_ram_0001651a_caseD_16:
      DAT_ram_200001d8 = '\x18';
      break;
    case '\x17':
      FUN_ram_00016242();
switchD_ram_00016762_caseD_19:
      DAT_ram_200001d8 = '\x19';
      break;
    case '\x18':
switchD_ram_0001651a_caseD_18:
      DAT_ram_200001d8 = '\x1a';
      break;
    case '\x19':
switchD_ram_0001651a_caseD_19:
      DAT_ram_200001d8 = '\x1b';
      break;
    case '\x1a':
      DAT_ram_200001d8 = '\x1c';
      break;
    case '\x1c':
      uVar3 = 5;
      if (display_layout == '\0') {
LAB_ram_0001676c:
        FUN_ram_00018ad2(0,6,uVar3);
      }
      else {
        iVar1 = settings_language_access(0,0);
        uVar3 = 5;
        if (iVar1 == 0) {
LAB_ram_00016782:
          FUN_ram_00019366(0,6,uVar3);
        }
      }
LAB_ram_00016714:
      DAT_ram_200001d8 = ' ';
      break;
    case '\x1e':
      DAT_ram_200001d8 = '\x1e';
      break;
    case '\x1f':
      uVar3 = 6;
      if (display_layout == '\0') {
LAB_ram_0001679e:
        FUN_ram_00018ad2(0,1,uVar3);
      }
      else {
        iVar1 = settings_language_access(0,0);
        uVar3 = 6;
        if (iVar1 == 0) {
LAB_ram_000167b4:
          FUN_ram_00019366(0,1,uVar3);
        }
      }
LAB_ram_00016732:
      DAT_ram_200001d8 = '\x01';
    }
  }
  else {
    if (button_event != '\x02') {
      return 0;
    }
    switch(DAT_ram_200001d8 + -1) {
    case '\0':
      if (display_layout == '\0') {
        uVar3 = 1;
        goto LAB_ram_0001676c;
      }
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar3 = 1;
        goto LAB_ram_00016782;
      }
      goto LAB_ram_00016714;
    case '\x01':
      goto switchD_ram_0001651a_caseD_2;
    case '\x02':
      goto switchD_ram_0001651a_caseD_3;
    case '\x03':
      goto switchD_ram_0001651a_caseD_1;
    case '\x04':
      if (display_layout == '\0') {
        uVar3 = 2;
        goto LAB_ram_0001679e;
      }
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar3 = 2;
        goto LAB_ram_000167b4;
      }
      goto LAB_ram_00016732;
    case '\x06':
      DAT_ram_200001d8 = '\x06';
      break;
    case '\a':
      goto switchD_ram_0001651a_caseD_5;
    case '\b':
      goto switchD_ram_0001651a_caseD_6;
    case '\t':
      goto switchD_ram_0001651a_caseD_7;
    case '\n':
      goto switchD_ram_0001651a_caseD_8;
    case '\v':
      if (display_layout == '\0') {
        uVar3 = 3;
        goto LAB_ram_000167d8;
      }
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar3 = 3;
        goto LAB_ram_000167f0;
      }
      goto LAB_ram_00016534;
    case '\f':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0xf,300,0x14,0);
        display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
        display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
        uVar3 = 4;
        goto LAB_ram_00016554;
      }
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        FUN_ram_00019366(0,3,4);
        display_fill_rectangle(0x83,0,0x140,0xf0,0);
        goto LAB_ram_000165ce;
      }
      goto LAB_ram_0001686c;
    case '\r':
      goto switchD_ram_0001651a_caseD_13;
    case '\x0e':
      goto switchD_ram_0001651a_caseD_14;
    case '\x14':
      DAT_ram_200001d8 = '\x14';
      break;
    case '\x15':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0xf,300,0x14,0);
        display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
        uVar3 = 0xda;
        uVar2 = 0x14;
        uVar4 = 0x8c;
      }
      else {
        uVar3 = 0xf0;
        uVar2 = 0;
        uVar4 = 0x83;
      }
      display_fill_rectangle(uVar4,uVar2,0x140,uVar3,0);
      FUN_ram_00014408(1);
      FUN_ram_000153d0();
      goto switchD_ram_0001651a_caseD_d;
    case '\x16':
      goto switchD_ram_00016762_caseD_16;
    case '\x17':
      goto switchD_ram_0001651a_caseD_15;
    case '\x18':
      FUN_ram_00015e9e();
      goto switchD_ram_0001651a_caseD_16;
    case '\x19':
      goto switchD_ram_00016762_caseD_19;
    case '\x1a':
      goto switchD_ram_0001651a_caseD_18;
    case '\x1b':
      goto switchD_ram_0001651a_caseD_19;
    case '\x1c':
      if (display_layout != '\0') {
        iVar1 = settings_language_access(0,0);
        if (iVar1 == 0) {
          uVar3 = 5;
          goto LAB_ram_0001696c;
        }
        goto LAB_ram_00016676;
      }
      display_fill_rectangle(0x96,0xf,300,0x14,0);
      display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
      display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
      uVar3 = 5;
LAB_ram_00016658:
      FUN_ram_00018ad2(0,4,uVar3);
      goto LAB_ram_00016660;
    case '\x1d':
      DAT_ram_200001d8 = '\x1f';
      break;
    case '\x1f':
      if (display_layout == '\0') {
        display_fill_rectangle(0x96,0xf,300,0x14,0);
        display_fill_rectangle(0x82,0x28,0x96,0xd0,0);
        display_fill_rectangle(0x8c,0x14,0x140,0xda,0);
        uVar3 = 6;
        goto LAB_ram_000166d0;
      }
      iVar1 = settings_language_access(0,0);
      if (iVar1 == 0) {
        uVar3 = 6;
        goto LAB_ram_000169c8;
      }
      goto LAB_ram_000166e8;
    }
  }
  button_event = 0;
  return 0;
}

