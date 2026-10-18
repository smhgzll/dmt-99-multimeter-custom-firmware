/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000fe08; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000fe08; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */

void FUN_ram_0000fe08(undefined4 param_1)

{
  char *pcVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  gp = &__global_pointer;
  switch(param_1) {
  case 0:
    uVar3 = 0;
    uVar2 = 0;
    if (display_layout == '\0') {
      pcVar1 = "0:/motif2/ncv0.jpg";
    }
    else {
      pcVar1 = "0:/ncv0.jpg";
    }
    break;
  case 1:
    resource_draw_jpeg("0:/ncv1.jpg",0,0);
    if (display_layout == '\0') {
      resource_draw_jpeg("0:/motif2/ncv1_up_1.jpg",0x5f,0x5a);
      resource_draw_jpeg("0:/motif2/ncv1_do_1.jpg",0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg("0:/motif2/ncv1_up_2.jpg",0x41,0x3c);
      resource_draw_jpeg("0:/motif2/ncv1_do_2.jpg",0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg("0:/motif2/ncv1_up_3.jpg",0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/motif2/ncv1_do_3.jpg";
    }
    else {
      resource_draw_jpeg("0:/ncv1_1.jpg",0x5f,0x5a);
      resource_draw_jpeg("0:/ncv1_1.jpg",0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg("0:/ncv1_2.jpg",0x41,0x3c);
      resource_draw_jpeg("0:/ncv1_2.jpg",0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg("0:/ncv1_3.jpg",0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/ncv1_3.jpg";
    }
    break;
  case 2:
    resource_draw_jpeg(&UNK_ram_00024d6c,0,0);
    if (display_layout == '\0') {
      resource_draw_jpeg("0:/motif2/ncv2_up_1.jpg",0x5f,0x5a);
      resource_draw_jpeg(&UNK_ram_00024d90,0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg("0:/motif2/ncv2_up_2.jpg",0x41,0x3c);
      resource_draw_jpeg(&UNK_ram_00024dc0,0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg("0:/motif2/ncv2_up_3.jpg",0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/motif2/ncv2_do_3.jpg";
    }
    else {
      resource_draw_jpeg(&UNK_ram_00024e08,0x5f,0x5a);
      resource_draw_jpeg(&UNK_ram_00024e08,0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024e18,0x41,0x3c);
      resource_draw_jpeg(&UNK_ram_00024e18,0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024e28,0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/ncv2_3.jpg";
    }
    break;
  case 3:
  case 4:
    resource_draw_jpeg(&UNK_ram_00024e38,0,0);
    if (display_layout == '\0') {
      resource_draw_jpeg(&UNK_ram_00024e44,0x5f,0x5a);
      resource_draw_jpeg(&UNK_ram_00024e5c,0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024e74,0x41,0x3c);
      resource_draw_jpeg(&UNK_ram_00024e8c,0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024ea4,0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/motif2/ncv3_do_3.jpg";
    }
    else {
      resource_draw_jpeg(&UNK_ram_00024ed4,0x5f,0x5a);
      resource_draw_jpeg(&UNK_ram_00024ed4,0x5f,0x8c);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024ee4,0x41,0x3c);
      resource_draw_jpeg(&UNK_ram_00024ee4,0x41,0xa0);
      delay_ms(200);
      resource_draw_jpeg(&UNK_ram_00024ef4,0x1e,0x14);
      uVar3 = 0xbe;
      uVar2 = 0x1e;
      pcVar1 = "0:/ncv3_3.jpg";
    }
    break;
  default:
    return;
  }
  resource_draw_jpeg(pcVar1,uVar2,uVar3);
  return;
}

