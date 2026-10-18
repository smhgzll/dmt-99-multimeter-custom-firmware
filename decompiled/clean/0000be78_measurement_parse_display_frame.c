/* Semantic review: high. 5AA5 header packed segment decoding decimal and unit flag extraction.
 * Address 0x0000be78; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000be78; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_12 replaced with injection: wch_save_12 */
/* high: 5AA5 header packed segment decoding decimal and unit flag extraction */

void measurement_parse_display_frame(void)

{
  uchar uVar1;
  bool bVar2;
  undefined4 uVar3;
  char cVar4;
  undefined1 uVar5;
  char cVar6;
  int iVar7;
  char cVar8;
  uchar auStack_4c [4];
  char acStack_48 [20];
  
  gp = &__global_pointer;
  if (measurement_dma_frame.first == 'Z') {
    if (measurement_dma_frame.second == 0xa5) {
      if ((measurement_dma_frame.status_or_segments0 & 2) == 0) {
        if ((measurement_dma_frame.status_or_segments0 & 1) == 0) {
          measurement_ac_dc_code = 0xff;
        }
        else {
          measurement_ac_dc_code = 1;
        }
      }
      else {
        measurement_ac_dc_code = 2;
      }
      uVar3 = 0x2d;
      if ((measurement_dma_frame.status_or_segments0 & 0x10) == 0) {
        uVar3 = 0x20;
      }
      measurement_decode_segments(auStack_4c);
      iVar7 = 0;
      bVar2 = false;
      uVar5 = DAT_ram_2000027c;
      do {
        uVar1 = auStack_4c[iVar7];
        if ((char)uVar1 < '\n') {
          acStack_48[iVar7] = uVar1 + '0';
          bVar2 = true;
          uVar5 = 1;
          goto LAB_ram_0000bf20;
        }
        if (uVar1 == '\n') {
          cVar4 = 'A';
LAB_ram_0000bfa4:
          acStack_48[iVar7] = cVar4;
LAB_ram_0000bfaa:
          bVar2 = true;
          uVar5 = 0;
        }
        else {
          if (uVar1 == '\v') {
            cVar4 = 'U';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 == '\f') {
            cVar4 = 'T';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 == '\r') {
            cVar4 = 'O';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 == '\x0e') {
            cVar4 = 'L';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 == '\x0f') {
            cVar4 = 'E';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 == '\x10') {
            cVar4 = '-';
            goto LAB_ram_0000bfa4;
          }
          if (uVar1 != '\x11') {
            if (uVar1 == '\x12') {
              acStack_48[iVar7] = 'C';
            }
            else if (uVar1 == '\x13') {
              acStack_48[iVar7] = 'P';
            }
            else if (uVar1 == '\x14') {
              acStack_48[iVar7] = 'S';
            }
            else {
              if (uVar1 != '\x15') {
                acStack_48[iVar7] = 'X';
                goto LAB_ram_0000bf20;
              }
              acStack_48[iVar7] = 'r';
            }
            goto LAB_ram_0000bfaa;
          }
          acStack_48[iVar7] = ' ';
          if (((measurement_mode == '\x12') || (measurement_mode == '\x11')) ||
             (measurement_mode == '\x16')) {
            acStack_48[iVar7] = '0';
          }
        }
LAB_ram_0000bf20:
        iVar7 = iVar7 + 1;
      } while (iVar7 != 4);
      if (bVar2) {
        DAT_ram_2000027c = uVar5;
      }
      cVar4 = acStack_48[2];
      cVar8 = acStack_48[3];
      if ((measurement_dma_frame.packed_segments[0] & 0x10) == 0) {
        cVar6 = acStack_48[1];
        if ((measurement_dma_frame.packed_segments[1] & 0x10) == 0) {
          cVar4 = acStack_48[3];
          cVar8 = 0x20;
          if ((measurement_dma_frame.packed_segments[2] & 0x10) != 0) {
            cVar4 = 0x2e;
            cVar8 = acStack_48[3];
          }
        }
        else {
          acStack_48[2] = 0x2e;
        }
      }
      else {
        cVar6 = 0x2e;
        acStack_48[2] = acStack_48[1];
      }
      FUN_ram_0000969e(&measurement_text,"%c%c%c%c%c%c%c",uVar3,acStack_48[0],cVar6,acStack_48[2],
                       cVar4,cVar8,0);
      measurement_text_pointer = &measurement_text;
      uVar3 = FUN_ram_00009774(&measurement_text);
      if ((measurement_dma_frame.unit_flags1 & 2) == 0) {
        if ((measurement_dma_frame.unit_flags0 & 0x20) == 0) {
          if ((measurement_dma_frame.unit_flags1 & 1) == 0) {
            if ((measurement_dma_frame.unit_flags0 & 0x10) == 0) {
              if ((measurement_dma_frame.unit_flags1 & 0x20) == 0) {
                if ((measurement_dma_frame.unit_flags1 & 4) == 0) {
                  measurement_unit_code = 1;
                }
                else if ((measurement_dma_frame.packed_segments[3] & 0x40) == 0) {
                  if ((measurement_dma_frame.unit_flags0 & 4) == 0) {
                    measurement_unit_code = 0x12;
                  }
                  else {
                    measurement_unit_code = 0x14;
                  }
                }
                else {
                  measurement_unit_code = 0x13;
                }
              }
              else {
                measurement_unit_code = 0x15;
              }
            }
            else if ((measurement_dma_frame.packed_segments[3] & 0x20) == 0) {
              if ((measurement_dma_frame.packed_segments[3] & 0x10) == 0) {
                measurement_unit_code = 0x11;
              }
              else {
                measurement_unit_code = 0x10;
              }
            }
            else {
              measurement_unit_code = 0xf;
            }
          }
          else if ((measurement_dma_frame.packed_segments[3] & 0x10) == 0) {
            if ((measurement_dma_frame.unit_flags0 & 1) == 0) {
              measurement_unit_code = 10;
            }
            else {
              measurement_unit_code = 9;
            }
          }
          else {
            measurement_unit_code = 8;
          }
        }
        else if ((measurement_dma_frame.packed_segments[3] & 0x40) == 0) {
          if ((measurement_dma_frame.unit_flags0 & 4) == 0) {
            measurement_unit_code = 0xb;
          }
          else {
            measurement_unit_code = 0xd;
          }
        }
        else {
          measurement_unit_code = 0xc;
        }
      }
      else if ((measurement_dma_frame.unit_flags0 & 1) == 0) {
        measurement_unit_code = 6;
      }
      else {
        measurement_unit_code = 7;
      }
      measurement_status_high = measurement_dma_frame.unit_flags0 >> 7;
      measurement_value_bits = uVar3;
      goto LAB_ram_0000be9e;
    }
  }
  uVar3 = 0;
LAB_ram_0000be9e:
  wch_restore_12(uVar3);
  return;
}

