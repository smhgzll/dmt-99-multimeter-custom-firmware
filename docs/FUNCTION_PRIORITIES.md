# Phase4 continuation function priorities

The original140 incomplete non-trap bodies are ranked below. **123 remain incomplete**; 17 have been completed in this continuation. Current partial entries remain included in the incomplete total.

Startup/application/menu/scheduling are first. The remaining-dependencies column uses preserved direct call edges; it excludes completed native/SDK/qualified-library counterparts. Static reachability is not a hardware execution claim. Source gaps and valid domains remain in `source_coverage.json`.

| Rank | Original entry | Role | Current | Remaining direct dependencies |
|---:|---|---|---|---|
| 1 | [0x00008ed4](../decompiled/clean/00008ed4_application_loop.c) | application_loop | full | 06608, 075f0, 07f66, 0a910, 0be24, 0c248, 14cd6, 1a64c |
| 2 | [0x0000908a](../decompiled/clean/0000908a_main.c) | main | full | 06630, 0770a, 087aa, 0cb8e, 0d2fc, 0dd4e, 1a626, 1a682 |
| 3 | [0x0000c248](../decompiled/clean/0000c248_settings_load_and_apply.c) | settings_load_and_apply | partial | 06630, 08016, 0969e, 15168 |
| 4 | [0x000169d4](../decompiled/clean/000169d4_menu_dispatch.c) | menu_dispatch | full | 06616, 0969e |
| 5 | [0x000164d4](../decompiled/clean/000164d4_FUN_ram_000164d4.c) | menu_previous_next | full | 06630 |
| 6 | [0x0001598c](../decompiled/clean/0001598c_FUN_ram_0001598c.c) | menu_enter_back | full | 06630, 08016 |
| 7 | [0x00015af6](../decompiled/clean/00015af6_FUN_ram_00015af6.c) | menu_select | full | 06616, 0969e |
| 8 | [0x00015168](../decompiled/clean/00015168_FUN_ram_00015168.c) | startup_choose_language | partial | 06616 |
| 9 | [0x00014cd6](../decompiled/clean/00014cd6_charge_screen_poll.c) | charge_screen_poll | partial | 06616, 0969e |
| 10 | [0x0000b5c8](../decompiled/clean/0000b5c8_measurement_regular_buttons.c) | measurement_regular_buttons | full | 06630 |
| 11 | [0x0000b9e0](../decompiled/clean/0000b9e0_measurement_record_buttons.c) | measurement_record_buttons | full | 06630, 0969e, 1b61e, 1c3b8, 1cfae, 1e2e2 |
| 12 | [0x0000bc90](../decompiled/clean/0000bc90_measurement_monitor_buttons.c) | measurement_monitor_buttons | full | 06630 |
| 13 | [0x0000a910](../decompiled/clean/0000a910_measurement_ui_regular_event.c) | measurement_ui_regular_event | partial | 06616, 0a7d0, 1aede |
| 14 | [0x000097f8](../decompiled/clean/000097f8_measurement_forward_button_command.c) | measurement_forward_button_command | full | 06630, 1a64c |
| 15 | [0x0000da48](../decompiled/clean/0000da48_settings_auto_off_access.c) | settings_auto_off_access | full | — |
| 16 | [0x0000d964](../decompiled/clean/0000d964_settings_language_access.c) | settings_language_access | full | 06630, 0969e |
| 17 | [0x0000dc46](../decompiled/clean/0000dc46_settings_restore_defaults.c) | settings_restore_defaults | full | 06630 |
| 18 | [0x0001a64c](../decompiled/clean/0001a64c_delay_ms.c) | delay_ms | untranslated | — |
| 19 | [0x000075f0](../decompiled/clean/000075f0_battery_percentage_estimate.c) | battery_percentage_estimate | untranslated | 06630, 07598, 1aede, 1d728, 1db48, 1dddc, 1e164, 1e21c, 1e2e2 |
| 20 | [0x00007f66](../decompiled/clean/00007f66_tim8_backlight_pwm_config.c) | tim8_backlight_pwm_config | partial | 06630 |
| 21 | [0x00008016](../decompiled/clean/00008016_FUN_ram_00008016.c) | backlight_apply_setting | partial | 06630, 07f66 |
| 22 | [0x0000cb94](../decompiled/clean/0000cb94_NVIC_Init.c) | NVIC_Init | untranslated | — |
| 23 | [0x00007562](../decompiled/clean/00007562_charge_detect_gpio_init.c) | charge_detect_gpio_init | untranslated | 06630 |
| 24 | [0x00007598](../decompiled/clean/00007598_adc_read_averaged.c) | adc_read_averaged | untranslated | 06616, 1a64c |
| 25 | [0x0000767a](../decompiled/clean/0000767a_adc_battery_init.c) | adc_battery_init | untranslated | 06630 |
| 26 | [0x0000770a](../decompiled/clean/0000770a_battery_inputs_init.c) | battery_inputs_init | untranslated | 06630, 07562, 0767a |
| 27 | [0x0000780c](../decompiled/clean/0000780c_tim2_buzzer_pwm_config.c) | tim2_buzzer_pwm_config | partial | 06630 |
| 28 | [0x0000792e](../decompiled/clean/0000792e_FUN_ram_0000792e.c) | buzzer_beep_500ms | full | 06630, 1a64c |
| 29 | [0x000087aa](../decompiled/clean/000087aa_display_init.c) | display_init | partial | 06630, 1a64c |
| 30 | [0x0000cb8e](../decompiled/clean/0000cb8e_NVIC_PriorityGroupConfig.c) | NVIC_PriorityGroupConfig | untranslated | — |
| 31 | [0x0000d212](../decompiled/clean/0000d212_SystemInit.c) | SystemInit | untranslated | — |
| 32 | [0x0000d2fc](../decompiled/clean/0000d2fc_SystemCoreClockUpdate.c) | SystemCoreClockUpdate | untranslated | — |
| 33 | [0x0000dd4e](../decompiled/clean/0000dd4e_filesystem_resources_init.c) | filesystem_resources_init | untranslated | 06630, 0d86c, 0fb48 |
| 34 | [0x0001a626](../decompiled/clean/0001a626_delay_init.c) | delay_init | untranslated | — |
| 35 | [0x0001a682](../decompiled/clean/0001a682_debug_usart2_init.c) | debug_usart2_init | untranslated | 06630 |
| 36 | [0x0000c668](../decompiled/clean/0000c668_handle_reset.c) | handle_reset | untranslated | 0d212 |
| 37 | [0x00006400](../decompiled/clean/00006400_application_entry.c) | application_entry | untranslated | — |
| 38 | [0x0000a7a4](../decompiled/clean/0000a7a4_FUN_ram_0000a7a4.c) | measurement_dispatch_special_display | untranslated | 06630, 0a74e, 0a782 |
| 39 | [0x0000be18](../decompiled/clean/0000be18_measurement_get_unit_code.c) | measurement_get_unit_code | untranslated | — |
| 40 | [0x0000be1e](../decompiled/clean/0000be1e_measurement_get_value_bits.c) | measurement_get_value_bits | untranslated | — |
| 41 | [0x0000a74e](../decompiled/clean/0000a74e_FUN_ram_0000a74e.c) | ncv_decode_dash_level | untranslated | 06630 |
| 42 | [0x0000a782](../decompiled/clean/0000a782_FUN_ram_0000a782.c) | live_decode_symbol | untranslated | 06630 |
| 43 | [0x0000a7d0](../decompiled/clean/0000a7d0_FUN_ram_0000a7d0.c) | auto_mode_label_update | untranslated | 06630 |
| 44 | [0x0000b56a](../decompiled/clean/0000b56a_FUN_ram_0000b56a.c) | diagnostic_entry_commands | full | 06630, 1a64c |
| 45 | [0x0000be24](../decompiled/clean/0000be24_FUN_ram_0000be24.c) | measurement_get_text_pointer | untranslated | — |
| 46 | [0x0000be2a](../decompiled/clean/0000be2a_measurement_overload_kind.c) | measurement_overload_kind | untranslated | — |
| 47 | [0x0001a336](../decompiled/clean/0001a336_FUN_ram_0001a336.c) | menu_draw_all_threshold_items | full | 06630 |
| 48 | [0x0000e17c](../decompiled/clean/0000e17c_FUN_ram_0000e17c.c) | fat_move_window | untranslated | 06630, 0e134 |
| 49 | [0x0000e71e](../decompiled/clean/0000e71e_FUN_ram_0000e71e.c) | fat_get_cluster_entry | untranslated | 06616, 0e17c |
| 50 | [0x0000e7fc](../decompiled/clean/0000e7fc_FUN_ram_0000e7fc.c) | fat_directory_seek | untranslated | 06616, 0e71e |
| 51 | [0x00007e20](../decompiled/clean/00007e20_display_begin_pixels.c) | display_begin_pixels | untranslated | 06630 |
| 52 | [0x0000d498](../decompiled/clean/0000d498_spi_flash_send_byte.c) | spi_flash_send_byte | untranslated | 06630, 0d442 |
| 53 | [0x0000eb26](../decompiled/clean/0000eb26_FUN_ram_0000eb26.c) | fat_directory_next | untranslated | 06616, 0e71e, 0e8b2, 0eac8 |
| 54 | [0x0000d442](../decompiled/clean/0000d442_spi1_exchange_byte.c) | spi1_exchange_byte | untranslated | 06630 |
| 55 | [0x0000df10](../decompiled/clean/0000df10_FUN_ram_0000df10.c) | fat_validate_object | untranslated | 06630, 0dca0 |
| 56 | [0x0000e2b8](../decompiled/clean/0000e2b8_FUN_ram_0000e2b8.c) | fat_find_and_mount_volume | untranslated | 06608, 0dca0, 0dca4, 0e17c, 0e1bc |
| 57 | [0x0000c320](../decompiled/clean/0000c320_power_down.c) | power_down | partial | 06630, 07f66, 0d87c |
| 58 | [0x0000e134](../decompiled/clean/0000e134_FUN_ram_0000e134.c) | fat_sync_window | untranslated | 06630 |
| 59 | [0x0000ea1e](../decompiled/clean/0000ea1e_FUN_ram_0000ea1e.c) | fat_sync_filesystem | untranslated | 06630, 0e134 |
| 60 | [0x0000dca0](../decompiled/clean/0000dca0_disk_status.c) | disk_status | untranslated | — |
| 61 | [0x0000dca4](../decompiled/clean/0000dca4_disk_initialize.c) | disk_initialize | untranslated | — |
| 62 | [0x0000dd48](../decompiled/clean/0000dd48_FUN_ram_0000dd48.c) | filesystem_fixed_timestamp | untranslated | — |
| 63 | [0x0000e086](../decompiled/clean/0000e086_FUN_ram_0000e086.c) | fat_path_decode_character | untranslated | 06630, 0fcfc |
| 64 | [0x0000e5e8](../decompiled/clean/0000e5e8_FUN_ram_0000e5e8.c) | fat_put_cluster_entry | untranslated | 06616, 0e17c |
| 65 | [0x0000e8b2](../decompiled/clean/0000e8b2_FUN_ram_0000e8b2.c) | fat_allocate_chain | untranslated | 06616, 0e5e8, 0e71e |
| 66 | [0x0000e9a4](../decompiled/clean/0000e9a4_FUN_ram_0000e9a4.c) | fat_remove_chain | untranslated | 06616, 0e5e8, 0e71e |
| 67 | [0x0000ebf0](../decompiled/clean/0000ebf0_FUN_ram_0000ebf0.c) | fat_directory_allocate | untranslated | 06616, 0e17c, 0e7fc, 0eb26 |
| 68 | [0x0000ec3e](../decompiled/clean/0000ec3e_FUN_ram_0000ec3e.c) | fat_directory_read | untranslated | 06608, 0e17c, 0eb26 |
| 69 | [0x0000edde](../decompiled/clean/0000edde_FUN_ram_0000edde.c) | fat_directory_find | untranslated | 06600, 0663c, 0e17c, 0e7fc, 0eb26, 1e57a |
| 70 | [0x0000ef70](../decompiled/clean/0000ef70_FUN_ram_0000ef70.c) | fat_create_name | untranslated | 06600, 0663c, 0e086, 0e7fc, 0edde, 0fc9e, 1e7f4 |
| 71 | [0x0000fc9e](../decompiled/clean/0000fc9e_FUN_ram_0000fc9e.c) | fat_unicode_to_codepage | partial | — |
| 72 | [0x00007e0a](../decompiled/clean/00007e0a_display_write_register.c) | display_write_register | untranslated | 06630 |
| 73 | [0x00007e68](../decompiled/clean/00007e68_FUN_ram_00007e68.c) | display_scan_direction | full | 06630, 07e0a |
| 74 | [0x0000d4a4](../decompiled/clean/0000d4a4_spi_flash_receive_byte.c) | spi_flash_receive_byte | untranslated | 06630, 0d442 |
| 75 | [0x0000d782](../decompiled/clean/0000d782_FUN_ram_0000d782.c) | spi_flash_detect_geometry | untranslated | 06630 |
| 76 | [0x0000d86c](../decompiled/clean/0000d86c_spi_flash_init.c) | spi_flash_init | untranslated | 06630, 0d782 |
| 77 | [0x0000d87c](../decompiled/clean/0000d87c_settings_commit_internal_flash.c) | settings_commit_internal_flash | partial | 06616 |
| 78 | [0x0000d908](../decompiled/clean/0000d908_read_halfwords.c) | read_halfwords | untranslated | — |
| 79 | [0x0000e1bc](../decompiled/clean/0000e1bc_FUN_ram_0000e1bc.c) | fat_check_volume_boot | untranslated | 06630, 0e17c, 1e57a |
| 80 | [0x0000eac8](../decompiled/clean/0000eac8_FUN_ram_0000eac8.c) | fat_clear_cluster | untranslated | 06616, 0e134 |
| 81 | [0x0000ed8e](../decompiled/clean/0000ed8e_FUN_ram_0000ed8e.c) | fat_directory_remove | untranslated | 06616, 0e17c, 0e7fc, 0eb26 |
| 82 | [0x0000f28a](../decompiled/clean/0000f28a_FUN_ram_0000f28a.c) | fat_directory_register | untranslated | 06608, 0e17c, 0e7fc, 0eb26, 0ebf0, 0edde |
| 83 | [0x0000f8a4](../decompiled/clean/0000f8a4_FUN_ram_0000f8a4.c) | fat_sync_file | untranslated | 06630, 0dd48, 0df10, 0e17c, 0ea1e |
| 84 | [0x0000fab0](../decompiled/clean/0000fab0_FUN_ram_0000fab0.c) | fat_unlink_path | untranslated | 06630, 0e2b8, 0e7fc, 0e9a4, 0ea1e, 0ec3e, 0ed8e, 0ef70 |
| 85 | [0x0000fb48](../decompiled/clean/0000fb48_FUN_ram_0000fb48.c) | fat_make_directory | untranslated | 06608, 0e086, 0e2b8, 0e7fc, 0ea1e, 0ebf0, 0ec3e, 0fc9e, 1e7f4 |
| 86 | [0x0000fcfc](../decompiled/clean/0000fcfc_FUN_ram_0000fcfc.c) | fat_codepage_to_unicode | partial | — |
| 87 | [0x0000cb88](../decompiled/clean/0000cb88_NMI_Handler.c) | NMI_Handler | untranslated | — |
| 88 | [0x0000969e](../decompiled/clean/0000969e_FUN_ram_0000969e.c) | format_string_varargs | partial | 0910e |
| 89 | [0x0001a74a](../decompiled/clean/0001a74a_FUN_ram_0001a74a.c) | debug_format_emit | untranslated | 1a6e2 |
| 90 | [0x0001a794](../decompiled/clean/0001a794_FUN_ram_0001a794.c) | debug_format_integer_digits | untranslated | 1a74a |
| 91 | [0x0001a966](../decompiled/clean/0001a966_FUN_ram_0001a966.c) | debug_format_numeric_family | untranslated | 1a74a, 1a794, 1af02, 1b294 |
| 92 | [0x0001aede](../decompiled/clean/0001aede_FUN_ram_0001aede.c) | debug_printf | untranslated | 1ab86 |
| 93 | [0x000090d2](../decompiled/clean/000090d2_strlen.c) | strlen | untranslated | — |
| 94 | [0x0000910e](../decompiled/clean/0000910e_FUN_ram_0000910e.c) | format_buffer_engine | partial | 06600, 0663c, 090d2, 1b61e, 1c302, 1c3b8, 1c870, 1d016, 1d072, 1d0e4 |
| 95 | [0x0001a6e2](../decompiled/clean/0001a6e2_debug_putchar.c) | debug_putchar | untranslated | 06616 |
| 96 | [0x0001a720](../decompiled/clean/0001a720_FUN_ram_0001a720.c) | heap_adjust_break | untranslated | — |
| 97 | [0x0001a874](../decompiled/clean/0001a874_FUN_ram_0001a874.c) | debug_format_integer_field | untranslated | 1a74a, 1a794 |
| 98 | [0x0001aa86](../decompiled/clean/0001aa86_FUN_ram_0001aa86.c) | debug_format_float_family | untranslated | 1a74a, 1a966, 1c302, 1c3b8, 1c870, 1d14c, 1d208 |
| 99 | [0x0001ab86](../decompiled/clean/0001ab86_FUN_ram_0001ab86.c) | debug_format_engine | untranslated | 1a74a, 1a794, 1a874, 1a966, 1aa86 |
| 100 | [0x00006630](../decompiled/clean/00006630_wch_save_2.c) | wch_save_2 | untranslated | — |
| 101 | [0x00006616](../decompiled/clean/00006616_wch_save_6.c) | wch_save_6 | untranslated | — |
| 102 | [0x00006600](../decompiled/clean/00006600_wch_save_12.c) | wch_save_12 | untranslated | — |
| 103 | [0x0000663c](../decompiled/clean/0000663c_wch_restore_12.c) | wch_restore_12 | untranslated | — |
| 104 | [0x00006608](../decompiled/clean/00006608_wch_save_10.c) | wch_save_10 | untranslated | — |
| 105 | [0x0001e4f8](../decompiled/clean/0001e4f8_FUN_ram_0001e4f8.c) | runtime_count_leading_zeros | untranslated | — |
| 106 | [0x0001c3b8](../decompiled/clean/0001c3b8_FUN_ram_0001c3b8.c) | runtime_multiply_double | untranslated | 1e4f8 |
| 107 | [0x0001e2e2](../decompiled/clean/0001e2e2_FUN_ram_0001e2e2.c) | runtime_float_to_double | untranslated | 1e4f8 |
| 108 | [0x0001b61e](../decompiled/clean/0001b61e_FUN_ram_0001b61e.c) | runtime_add_double | untranslated | 1e4f8 |
| 109 | [0x0001dabc](../decompiled/clean/0001dabc_FUN_ram_0001dabc.c) | runtime_float_compare_le | untranslated | — |
| 110 | [0x0001d728](../decompiled/clean/0001d728_FUN_ram_0001d728.c) | runtime_divide_float | untranslated | 1e4f8 |
| 111 | [0x0001da36](../decompiled/clean/0001da36_FUN_ram_0001da36.c) | runtime_float_compare_ge | untranslated | — |
| 112 | [0x0001db48](../decompiled/clean/0001db48_FUN_ram_0001db48.c) | runtime_multiply_float | untranslated | 1e4f8 |
| 113 | [0x0001e21c](../decompiled/clean/0001e21c_FUN_ram_0001e21c.c) | runtime_signed_int_to_float | untranslated | 1e4f8 |
| 114 | [0x0001d072](../decompiled/clean/0001d072_FUN_ram_0001d072.c) | runtime_signed_int_to_double | untranslated | 1e4f8 |
| 115 | [0x0001dddc](../decompiled/clean/0001dddc_FUN_ram_0001dddc.c) | runtime_subtract_float | untranslated | 1e4f8 |
| 116 | [0x0001e38e](../decompiled/clean/0001e38e_FUN_ram_0001e38e.c) | runtime_double_to_float | untranslated | — |
| 117 | [0x0001c870](../decompiled/clean/0001c870_FUN_ram_0001c870.c) | runtime_subtract_double | untranslated | 1e4f8 |
| 118 | [0x0001cfae](../decompiled/clean/0001cfae_FUN_ram_0001cfae.c) | runtime_double_to_signed_int | untranslated | — |
| 119 | [0x0001d016](../decompiled/clean/0001d016_FUN_ram_0001d016.c) | runtime_double_to_unsigned_int | untranslated | — |
| 120 | [0x0001d9da](../decompiled/clean/0001d9da_FUN_ram_0001d9da.c) | runtime_float_not_equal | untranslated | — |
| 121 | [0x0001c302](../decompiled/clean/0001c302_FUN_ram_0001c302.c) | runtime_compare_double | untranslated | — |
| 122 | [0x0001d3be](../decompiled/clean/0001d3be_FUN_ram_0001d3be.c) | runtime_add_float | untranslated | 1e4f8 |
| 123 | [0x0001e1c6](../decompiled/clean/0001e1c6_FUN_ram_0001e1c6.c) | runtime_float_to_unsigned_int | untranslated | — |
| 124 | [0x0001e57a](../decompiled/clean/0001e57a_FUN_ram_0001e57a.c) | runtime_memcmp | untranslated | — |
| 125 | [0x0001e7f4](../decompiled/clean/0001e7f4_FUN_ram_0001e7f4.c) | runtime_strchr | untranslated | — |
| 126 | [0x0001e81c](../decompiled/clean/0001e81c_FUN_ram_0001e81c.c) | heap_lock_noop | untranslated | — |
| 127 | [0x0001e81e](../decompiled/clean/0001e81e_FUN_ram_0001e81e.c) | heap_unlock_noop | untranslated | — |
| 128 | [0x0001af02](../decompiled/clean/0001af02_FUN_ram_0001af02.c) | runtime_unsigned_divide_64 | untranslated | — |
| 129 | [0x0001b294](../decompiled/clean/0001b294_FUN_ram_0001b294.c) | runtime_unsigned_remainder_64 | untranslated | — |
| 130 | [0x0001bd4e](../decompiled/clean/0001bd4e_FUN_ram_0001bd4e.c) | runtime_divide_double | untranslated | 1e4f8 |
| 131 | [0x0001d0e4](../decompiled/clean/0001d0e4_FUN_ram_0001d0e4.c) | runtime_unsigned_int_to_double | untranslated | 1e4f8 |
| 132 | [0x0001d14c](../decompiled/clean/0001d14c_FUN_ram_0001d14c.c) | runtime_double_to_unsigned_64 | untranslated | — |
| 133 | [0x0001d208](../decompiled/clean/0001d208_FUN_ram_0001d208.c) | runtime_unsigned_64_to_double | untranslated | 1e4f8 |
| 134 | [0x0001e164](../decompiled/clean/0001e164_FUN_ram_0001e164.c) | runtime_float_to_signed_int | untranslated | — |
| 135 | [0x0001e566](../decompiled/clean/0001e566_FUN_ram_0001e566.c) | heap_malloc_wrapper | untranslated | 1e6f4 |
| 136 | [0x0001e570](../decompiled/clean/0001e570_FUN_ram_0001e570.c) | heap_free_wrapper | untranslated | 1e650 |
| 137 | [0x0001e650](../decompiled/clean/0001e650_FUN_ram_0001e650.c) | heap_free_reentrant | untranslated | 1e81c, 1e81e |
| 138 | [0x0001e6f4](../decompiled/clean/0001e6f4_FUN_ram_0001e6f4.c) | heap_malloc_reentrant | untranslated | 1e7c2, 1e81c, 1e81e |
| 139 | [0x0001e7c2](../decompiled/clean/0001e7c2_FUN_ram_0001e7c2.c) | heap_sbrk_reentrant | untranslated | 1a720 |
| 140 | [0x0001e80c](../decompiled/clean/0001e80c_FUN_ram_0001e80c.c) | runtime_strcpy | untranslated | — |

Machine-readable roles, body sizes, callers, callees, reachability, tiers and current barriers: `analysis/incomplete_priority.json`.

Bootloader, actual USB MSC and internal Flash layout confirmation are deferred. RDP does not block reconstruction of application instructions already present in V2.4. Unknown CP936 physical reads, target timing and calibration remain unresolved.
