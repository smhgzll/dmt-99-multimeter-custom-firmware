/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x00019366; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00019366; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */

void FUN_ram_00019366(int param_1,int param_2,int param_3)

{
  short sVar1;
  short sVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined *puVar5;
  int iVar6;
  undefined4 uVar7;
  
  gp = &__global_pointer;
  if (param_1 != 0) {
    sVar1 = 0x27;
    do {
      sVar2 = sVar1 + 0x27;
      FUN_ram_00008050(0,sVar1,0x81,sVar1,0x4228);
      sVar1 = sVar2;
    } while (sVar2 != 0xea);
    FUN_ram_00008050(0x82,0,0x82,0xea,0x4228);
    FUN_ram_00008c64(5,9,&DAT_ram_000253d0,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x25,9,&DAT_ram_000253d4,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x45,9,&DAT_ram_000253d8,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x65,9,&DAT_ram_000253dc,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(5,0x30,&DAT_ram_000253e0,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x25,0x30,&DAT_ram_00025464,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x45,0x30,&DAT_ram_000253e4,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x65,0x30,&DAT_ram_000253e8,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(5,0x57,&DAT_ram_000253ec,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x25,0x57,&DAT_ram_000253f0,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x45,0x57,&DAT_ram_000253f4,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x65,0x57,&DAT_ram_000253f8,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(5,0x7e,&DAT_ram_000253fc,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x25,0x7e,&DAT_ram_00025400,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x45,0x7e,&DAT_ram_000253e4,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x65,0x7e,&DAT_ram_000253e8,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0,0xa5,&DAT_ram_00025404,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x15,0xa5,&DAT_ram_00025408,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x2a,0xa5,&DAT_ram_0002540c,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x3f,0xa5,&DAT_ram_00025410,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x54,0xa5,&DAT_ram_000253e4,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x69,0xa5,&DAT_ram_000253e8,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(5,0xcc,&DAT_ram_00025414,0xffff,0xffff,0x18,1);
    FUN_ram_00008c64(0x65,0xcc,&DAT_ram_00025418,0xffff,0xffff,0x18,1);
  }
  iVar6 = 2;
  uVar7 = 0;
  display_fill_rectangle
            (0,(param_2 + -1) * 0x27 + 1U & 0xffff,0x80,param_2 * 0x27 - 2U & 0xffff,0xfd40);
  do {
    switch(param_2 - 1U & 0xff) {
    case 0:
      FUN_ram_00008c64(5,9,&DAT_ram_000253d0,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x25,9,&DAT_ram_000253d4,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x45,9,&DAT_ram_000253d8,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_000253dc;
      uVar4 = 9;
      break;
    case 1:
      FUN_ram_00008c64(5,0x30,&DAT_ram_000253e0,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x25,0x30,&DAT_ram_00025464,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x45,0x30,&DAT_ram_000253e4,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_000253e8;
      uVar4 = 0x30;
      break;
    case 2:
      FUN_ram_00008c64(5,0x57,&DAT_ram_000253ec,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x25,0x57,&DAT_ram_000253f0,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x45,0x57,&DAT_ram_000253f4,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_000253f8;
      uVar4 = 0x57;
      break;
    case 3:
      FUN_ram_00008c64(5,0x7e,&DAT_ram_000253fc,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x25,0x7e,&DAT_ram_00025400,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x45,0x7e,&DAT_ram_000253e4,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_000253e8;
      uVar4 = 0x7e;
      break;
    case 4:
      FUN_ram_00008c64(0,0xa5,&DAT_ram_00025404,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x15,0xa5,&DAT_ram_00025408,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x2a,0xa5,&DAT_ram_0002540c,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x3f,0xa5,&DAT_ram_00025410,uVar7,uVar7,0x18,1);
      FUN_ram_00008c64(0x54,0xa5,&DAT_ram_000253e4,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_000253e8;
      uVar4 = 0xa5;
      uVar3 = 0x69;
      goto LAB_ram_0001991c;
    case 5:
      FUN_ram_00008c64(5,0xcc,&DAT_ram_00025414,uVar7,uVar7,0x18,1);
      puVar5 = &DAT_ram_00025418;
      uVar4 = 0xcc;
      break;
    default:
      goto switchD_ram_000196d8_default;
    }
    uVar3 = 0x65;
LAB_ram_0001991c:
    FUN_ram_00008c64(uVar3,uVar4,puVar5,uVar7,uVar7,0x18,1);
switchD_ram_000196d8_default:
    if ((iVar6 == 1) || (param_3 == 0)) {
      wch_restore_12();
      return;
    }
    display_fill_rectangle
              (0,(param_3 + -1) * 0x27 + 1U & 0xffff,0x80,param_3 * 0x27 - 2U & 0xffff,0);
    uVar7 = 0xffff;
    iVar6 = 1;
    param_2 = param_3;
  } while( true );
}

