#!/usr/bin/env python3
"""Curated, address-specific interpretations supported by instructions and SDK source."""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
ROWS='''
6400 application_entry high Entry JAL to verified reset routine
c668 handle_reset high AUIPC SRAM initialization CSR setup and mepc handoff
908a main high Startup mepc target initializes drivers then calls nonreturning application
8ed4 application_loop high Three UI branches selected by 0x200001de plus packet processing and power checks
6600 wch_save_12 high GCC t0-return register-save helper; 64-byte frame
6608 wch_save_10 high GCC t0-return register-save helper; 48-byte frame
6616 wch_save_6 high GCC t0-return register-save helper; 32-byte frame
6630 wch_save_2 high GCC t0-return register-save helper; 16-byte frame
663c wch_restore_12 high Shared restore fallthroughs at 0x6640 0x664a 0x6654
6660 memset_recovered high Repeated byte/word fill and length; computed tail branch
678c resource_draw_jpeg high Opens path then invokes decoder with output callback and display coordinates
6708 jpeg_input_callback high File read/seek adapter passed to decoder
6734 jpeg_display_callback high Pixel rectangle callback passed to decoder
696e jpeg_decoder_prepare medium Called with 0x1c00 work area and input callback before pixel decoding
6d9c jpeg_decoder_decompress medium Decoder calls rectangle callback; JPEG preparation relationship
7562 charge_detect_gpio_init high GPIOA pin 8 floating input
7598 adc_read_averaged high Repeated ADC conversion polling and averaging
75f0 battery_percentage_estimate medium ADC channel 10 and voltage diagnostics; float conversion signatures unresolved
767a adc_battery_init high ADC1 initialization calibration and PC0 analog input
770a battery_inputs_init medium Calls ADC and GPIO initialization routines
7736 tim3_periodic_init high TIM3 base values 9999 and 14399; update interrupt setup
7780 TIM3_IRQHandler high Vector slot 45; TIM3 update flag and periodic power management
780c tim2_buzzer_pwm_config medium TIM2 PWM on PA1; disable counterpart drives pin low
78d0 buzzer_enable medium TIM2 compare output and PA1 level control
7944 buttons_gpio_exti_init high PC2 8 9 10 11 12 pullups and falling-edge EXTI mapping
7b08 button_wait_longpress high Polls five PC inputs at 10ms intervals with thresholds 41 and 101
7c0a EXTI2_IRQHandler high Vector slot 24; PC2 debounce event 8 and conditional power-off
7c42 EXTI9_5_IRQHandler high Vector slot 39; PC9 event 3 and PC8 short 5 long 6
7cd8 EXTI15_10_IRQHandler high Vector slot 56; PC10 11 12 events 1 2 4
7d6a display_write_command high PC5 low PB output word PC13 write strobe PC4 select
7dba display_write_data high PC5 high PB output word PC13 write strobe PC4 select
7e0a display_write_register high Command then data transport
7e20 display_begin_pixels medium Writes configured memory-write command
7f18 display_set_orientation high 320x240 geometry and orientation command selection
7f66 tim8_backlight_pwm_config medium TIM8 PWM on PC6; duty used on display startup and power-off
876a display_fill high Width times height repeated color writes after window setup
87aa display_init high GPIOB data port PC control pins reset and LCD initialization commands
8a02 display_fill_rectangle high Window bounds followed by repeated RGB565 color writes
90d2 strlen high Byte loop ending at zero and return length
97c4 measurement_send_command high Exactly ten USART1 bytes with transfer-complete polling
97f8 measurement_forward_button_command high Builds AA55 template for IDs 1 3 8 9 from button events
98aa measurement_segment_decode high Masks 0x10 then matches explicit seven-segment patterns
997e measurement_decode_segments high Combines high/low nibbles across bytes 2 through 6 into four symbols
a910 measurement_ui_regular_event high 0xfd event dispatch and resource paths for measurement modes
aed0 measurement_ui_record_event high 0xfd event dispatch in second application branch
b29e measurement_ui_monitor_event medium Third application branch event processing and threshold UI
b5c8 measurement_regular_buttons high Button events select documented command IDs and menu state
b9e0 measurement_record_buttons high Command construction plus history append from event 5
bc90 measurement_monitor_buttons medium Third application branch command construction
be18 measurement_get_unit_code high Returns 0x20000289
be1e measurement_get_value_bits high Returns raw 32-bit float representation at 0x20000284
be2a measurement_overload_kind high String positions distinguish two OL representations returning 1 or 2
be78 measurement_parse_display_frame high 5AA5 header packed segment decoding decimal and unit flag extraction
c248 settings_load_and_apply high Loads packed settings and applies theme language layout and auto-off
c320 power_down high Saves settings dims backlight drives PC1 low then infinite loop
c350 measurement_uart_dma_init high USART1 PA9 PA10 13-byte circular DMA1 channel 5
c468 USART1_IRQHandler high Vector slot 53; IDLE clears and rearms DMA plus 13-byte event snapshot
c570 DMA1_Channel5_IRQHandler high Vector slot 31; DMA transfer-complete flag handling
c6f6 ADC_Init high Register masks and struct offsets correspond to official WCH ADC_Init
c74c ADC_Cmd high ADC CTLR2 bit 0 enable
c75c ADC_ResetCalibration high ADC CTLR2 reset-calibration bit
c766 ADC_GetResetCalibrationStatus high ADC CTLR2 reset-calibration status
c76e ADC_StartCalibration high ADC CTLR2 calibration bit
c778 ADC_GetCalibrationStatus high ADC CTLR2 calibration status
c780 ADC_SoftwareStartConvCmd high ADC CTLR2 mask 0x500000
c798 ADC_RegularChannelConfig high ADC sample time and regular sequence register selection
c850 ADC_GetConversionValue high ADC data register halfword read
c858 ADC_GetFlagStatus high ADC status mask test
c862 DMA_Init high Channel peripheral memory count and CFGR struct fields
c89a DMA_Cmd high DMA CFGR enable bit
c8ae DMA_ITConfig high DMA CFGR interrupt mask enable
c8c0 DMA_SetCurrDataCounter high DMA CNTR write
c8c4 DMA_ClearITPendingBit high DMA interrupt-clear registers selected by encoded flag
c8f0 EXTI_Init high EXTI rising falling and interrupt-event mode masks
c95a EXTI_GetITStatus high EXTI enable plus pending tests
c978 EXTI_ClearITPendingBit high EXTI pending write-one-to-clear
c982 FLASH_Unlock high Internal flash two-key unlock sequence
c99c FLASH_Lock high Internal flash CTLR lock bit
c9aa FLASH_ClearFlag high Internal flash STATR write
c9b2 FLASH_GetStatus high Internal flash busy and error status logic
c9ca FLASH_WaitForLastOperation high Polls internal flash status with timeout
c9ee FLASH_ProgramHalfWord high Internal flash program bit halfword store wait and disable
ca26 FLASH_Unlock_Fast high Fast-mode two-key unlock
ca44 FLASH_Lock_Fast high Fast-mode lock bit
ca52 FLASH_ErasePage_Fast high Fast erase with address rounded to 256-byte boundary
ca80 GPIO_Init high GPIO configuration nibbles and pullup-pulldown register logic
cb40 GPIO_ReadInputDataBit high GPIO INDR masked boolean read
cb4a GPIO_SetBits high GPIO BSHR write
cb4e GPIO_ResetBits high GPIO BCR write
cb52 GPIO_WriteBit high GPIO BSHR or BCR according to bit action
cb5c GPIO_EXTILineConfig high AFIO external interrupt port selector nibble
cb88 NMI_Handler high Vector slot 2; mret
cb8c HardFault_Handler high Vector slot 3; infinite self-loop
cb8e NVIC_PriorityGroupConfig high CSR priority group write
cb94 NVIC_Init high PFIC enable disable and priority configuration
cc3a RCC_ADCCLKConfig high RCC CFGR0 ADC prescaler field
cc4c RCC_GetClocksFreq high RCC clock mux PLL multiplier and bus prescaler calculations
cd52 RCC_AHBPeriphClockCmd high RCC AHB clock enable register
cd70 RCC_APB2PeriphClockCmd high RCC APB2 clock enable register
cd8e RCC_APB1PeriphClockCmd high RCC APB1 clock enable register
cdac SPI_Init high SPI configuration mask struct offsets CRC polynomial
cdea SPI_Cmd high SPI CTLR1 enable bit
ce04 SPI_I2S_SendData high SPI DATAR halfword store
ce08 SPI_I2S_ReceiveData high SPI DATAR halfword load
ce0c SPI_I2S_GetFlagStatus high SPI status flag mask
ce16 TIM_TimeBaseInit high TIM prescaler auto-reload and update generation
cec0 TIM_OC1Init high TIM channel 1 output compare registers
cf42 TIM_OC2Init high TIM channel 2 output compare registers
cff0 TIM_Cmd high TIM CTLR1 enable bit
d008 TIM_CtrlPWMOutputs high TIM BDTR master output enable
d01e TIM_ITConfig high TIM interrupt DMA enable register mask
d030 TIM_ARRPreloadConfig high TIM CTLR1 bit 7 auto-reload preload enable
d04a TIM_OC1PreloadConfig high TIM CHCTLR1 bit 3 output compare preload
d058 TIM_OC2PreloadConfig high TIM CHCTLR1 second channel preload
d06e TIM_SetCompare2 high TIM channel 2 compare halfword write
d072 TIM_GetITStatus high TIM interrupt enable and pending bits
d08a TIM_ClearITPendingBit high TIM status complement-mask write
d096 USART_Init high Official USART struct bit masks and baud divisor
d152 USART_Cmd high USART CTLR1 enable bit
d168 USART_ITConfig high Encoded USART interrupt register selection
d19e USART_DMACmd high USART CTLR3 DMA mask
d1b0 USART_SendData high USART DATAR write masked to nine bits
d1b8 USART_ReceiveData high USART DATAR read masked to nine bits
d1c0 USART_GetFlagStatus high USART STATR mask
d1ca USART_ClearFlag high USART STATR complement-mask write
d1d6 USART_GetITStatus high Encoded USART interrupt enable and status tests
d212 SystemInit high RCC 8MHz HSE times 18 PLL configuration for 144MHz
d2fc SystemCoreClockUpdate high RCC decode stores core clock at 0x200001cc
d38a spi1_flash_init high PA4 CS PA5 SCK PA6 MISO PA7 MOSI and SPI1 configuration
d442 spi1_exchange_byte high SPI1 TXE RXNE loops bounded by 201 polls
d498 spi_flash_send_byte high Wrapper around SPI1 exchange
d4a4 spi_flash_receive_byte high Wrapper exchanges dummy byte
d4b4 spi_flash_read_jedec_id high 0x9f and three response bytes with PA4 CS
d4f8 spi_flash_write_enable high 0x06 command under PA4 CS
d516 spi_flash_read_status high 0x05 command under PA4 CS
d538 spi_flash_erase_sector high 0x20 command and three address bytes with sector index shift
d588 spi_flash_page_program high 0x02 command 24-bit address and up to 256 data bytes
d602 spi_flash_program_pages high Splits writes at 256-byte page boundaries
d656 spi_flash_read high 0x03 command 24-bit address and dummy-clock reads
d6bc spi_flash_write_preserve_sector high 4096-byte read modify erase rewrite sequence
d86c spi_flash_init high Calls SPI1 initialization and JEDEC ID read
d87c settings_commit_internal_flash high Erases/programs 0x08040000 and copies 128 halfwords
d908 read_halfwords high 16-bit source-to-destination copy
d928 settings_load_internal_flash high Copies 128 halfwords from 0x08040000 to 0x20000020
d944 settings_theme_access high Packed word bits 8 and 9
d964 settings_language_access high Packed word low three bits and language resource path
d9e6 settings_field_3_4_access high Packed word bits 3 and 4; UI meaning not yet established
da04 settings_layout_access high Packed word bits 10 and 11; selects alternate layout
da28 settings_temperature_access high Packed word bit 5; temperature display call site
da48 settings_auto_off_access high Packed word bits 6 and 7; 0 300 900 1800 second thresholds
da9c history_append_measurement high Shifts 29 four-byte entries caps count at 30 and packs measurement
dbac history_decode_entry high Extracts mode unit sign scale and magnitude from selected four-byte entry
dc46 settings_restore_defaults medium Copies defaults and clears remaining 128-halfword settings buffer
dca0 disk_status medium FatFs-facing drive status wrapper returns zero
dca4 disk_initialize medium FatFs-facing drive initialization wrapper returns zero
dca8 disk_read high Sector index shifted by 12 then SPI read
dcd0 disk_write high Sector index shifted by 12 then preserved-sector SPI write
dd00 disk_ioctl high Sector count 2048 sector size 4096 erase block 1
dd4e filesystem_resources_init high Mount calls and BOOT resource handling
de12 filesystem_logo_remove high Mounts resources removes LOGO.jpg then remounts
14cd6 charge_screen_poll high GPIOA pin 8 controls recharge image loop
14d62 battery_icon_theme_a high Ten cell image paths selected by percentage thresholds
1511a application_show_initial_screen high Optional on.jpg splash then measurement screen setup
169d4 menu_dispatch medium Thirty-two state switch with settings history and about branches
1a626 delay_init high Core-clock-derived millisecond and microsecond factors
1a64c delay_ms high SysTick compare count enable and status polling
1a682 debug_usart2_init high PA2 alternate output USART2 at main argument 115200
1a6e2 debug_putchar high USART2 transmit register and TC polling
'''

GLOBALS='''
2000003e settings_word
20000040 history_count
20000042 history_records
200001c4 measurement_rx_suppressed
200001c6 measurement_event_consumed
200001cc system_core_clock_hz
200001de application_view
200001e8 jpeg_display_x
200001ea jpeg_display_y
200001ec battery_refresh_due
200001ed periodic_tick_count
200001ee button_event
200001ef display_orientation
20000205 display_layout
20000206 measurement_mode
20000208 auto_off_seconds
2000020a idle_seconds_counter
20000248 battery_percent
20000251 normal_operation
20000270 display_width
20000272 display_height
20000280 measurement_text_pointer
20000284 measurement_value_bits
20000288 measurement_ac_dc_code
20000289 measurement_unit_code
2000028a measurement_status_high
2000028c measurement_text
20000298 resource_language_prefix
200002ac measurement_dma_frame
200002bc measurement_saved_event
200002cc flash_sector_scratch
'''

def main():
    records={}
    known={int(f['address'],16) for f in json.loads((ROOT/'analysis/functions.json').read_text())}
    for line in ROWS.strip().splitlines():
        address,name,confidence,evidence=line.split(' ',3);a=int(address,16)
        if a not in known:continue
        records[f'0x{a:08x}']={'name':name,'confidence':confidence,'evidence':[evidence, f'Original instructions and pseudocode at 0x{a:08x}'],'notes':'Reconstructed semantic name; original symbol spelling is not known.'}
    vectors=json.loads((ROOT/'analysis/entrypoints.json').read_text())['vectors']
    for v in vectors:
        a=f"0x{int(v['target'],16):08x}"
        if v['kind']=='default_self_loop' and a not in records:
            name=v['sdk_name'] or f"vector_{v['index']}"
            records[a]={'name':name+'_default','confidence':'high','evidence':[f"Vector slot {v['index']} uses this target; original opcode is c.j to itself",'Slot identity follows official CH32V303 D8 startup table'],'notes':'Default trap loop; this interrupt is not demonstrated to be enabled.'}
    blob=(ROOT/'DMT-99_V2.4_20240306.bin').read_bytes()
    for a in sorted(known):
        key=f'0x{a:08x}'
        if 0xc5be<=a<0xc668 and key not in records and blob[a-0x6400:a-0x6400+2]==b'\x01\xa0':
            records[key]={'name':f'unused_default_trap_{a:x}','confidence':'high','category':'unreferenced_default_trap','evidence':['Two-byte self-loop inside the contiguous WCH weak-handler stub region; no vector points here'],'notes':'Retained weak stub; individual IRQ identity not assigned without a vector reference.'}
    (ROOT/'analysis/semantic_registry.json').write_text(json.dumps(records,indent=2)+'\n')
    (ROOT/'analysis/semantic_names.tsv').write_text(''.join(f"{a}\t{v['name']}\t{v['confidence']}: {v['evidence'][0]}\n" for a,v in records.items()))
    (ROOT/'analysis/global_names.tsv').write_text(''.join('\t'.join(line.split())+'\n' for line in GLOBALS.strip().splitlines()))
    print(f'Seeded {len(records)} evidence-backed function names')

if __name__=='__main__':main()
