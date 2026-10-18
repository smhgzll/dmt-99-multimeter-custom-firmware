/* Original address: 0x00014d62; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Ten cell image paths selected by percentage thresholds */

void battery_icon_theme_a(int param_1)

{
  char *pcVar1;
  
  gp = &__global_pointer;
  if (param_1 < 0x5b) {
    if (param_1 < 0x51) {
      if (param_1 < 0x47) {
        if (param_1 < 0x3d) {
          if (param_1 < 0x33) {
            if (param_1 < 0x29) {
              if (param_1 < 0x1f) {
                if (param_1 < 0x15) {
                  if (param_1 < 0xb) {
                    pcVar1 = "cell/cell0.jpg";
                  }
                  else {
                    pcVar1 = "cell/cell1.jpg";
                  }
                }
                else {
                  pcVar1 = "cell/cell2.jpg";
                }
              }
              else {
                pcVar1 = "cell/cell3.jpg";
              }
            }
            else {
              pcVar1 = "cell/cell4.jpg";
            }
          }
          else {
            pcVar1 = "cell/cell5.jpg";
          }
        }
        else {
          pcVar1 = "cell/cell6.jpg";
        }
      }
      else {
        pcVar1 = "cell/cell7.jpg";
      }
    }
    else {
      pcVar1 = "cell/cell8.jpg";
    }
  }
  else {
    pcVar1 = "cell/cell9.jpg";
  }
  resource_draw_jpeg(pcVar1,0x3c,1);
  return;
}

