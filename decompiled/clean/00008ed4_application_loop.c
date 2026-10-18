/* Semantic review: high. Three UI branches selected by 0x200001de plus packet processing and power checks.
 * Address 0x00008ed4; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00008ed4; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_10 replaced with injection: wch_save_10 */
/* high: Three UI branches selected by 0x200001de plus packet processing and power checks */

void application_loop(void)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  uint8_t *puVar4;
  int iVar5;
  
  uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x400);
  if ((uVar2 == 0) && (uVar2 = GPIO_ReadInputDataBit(&GPIOC,0x800), uVar2 == 0)) {
    normal_operation = 1;
    tim8_backlight_pwm_config(100,0x12bf,0);
    delay_ms(1000);
    FUN_ram_0000b56a();
    do {
      do {
        measurement_forward_button_command();
      } while (measurement_rx_suppressed != 0);
      if (measurement_event_consumed == 0) {
        measurement_event_consumed = 1;
      }
      else {
        measurement_parse_display_frame();
        uVar3 = FUN_ram_0000be24();
        FUN_ram_00008672(100,100,100,0x32,0x20,0xfd40,uVar3);
        FUN_ram_0000c11a(200,0x6e);
      }
      measurement_rx_suppressed = 0;
    } while( true );
  }
  settings_load_and_apply();
  application_show_initial_screen();
  if (application_view == '\x01') {
    measurement_mode = '\x14';
    button_event = 5;
  }
  else {
    measurement_mode = '\f';
    button_event = 3;
    delay_ms(500);
  }
  do {
    if (application_view == '\x01') {
      measurement_regular_buttons();
    }
    else if (application_view == '\x02') {
      measurement_record_buttons();
    }
    else if (application_view == '\x03') {
      measurement_monitor_buttons();
    }
    if (measurement_rx_suppressed == 0) {
      if (measurement_event_consumed == 0) {
        iVar5 = 0;
        do {
          puVar4 = &measurement_saved_event + iVar5;
          iVar1 = iVar5 + -3;
          iVar5 = iVar5 + 1;
          measurement_dma_frame.packed_segments[iVar1] = *puVar4;
        } while (iVar5 != 0x10);
        measurement_event_consumed = 1;
        DAT_ram_200001d9 = 0xff;
      }
      if (DAT_ram_2000023e == '\0') {
        measurement_parse_display_frame();
      }
      if (application_view == '\x01') {
        measurement_ui_regular_event();
      }
      else if (application_view == '\x02') {
        measurement_ui_record_event();
      }
      else if (application_view == '\x03') {
        measurement_ui_monitor_event();
      }
      measurement_rx_suppressed = 1;
    }
    charge_screen_poll();
    if (((battery_refresh_due == '\x01') && (battery_refresh_due = '\0', measurement_mode != '\x13')
        ) && (measurement_mode != '\t')) {
      battery_percent = battery_percentage_estimate();
      if (display_layout == '\0') {
        FUN_ram_00014e48();
      }
      else {
        battery_icon_theme_a();
      }
    }
  } while( true );
}

