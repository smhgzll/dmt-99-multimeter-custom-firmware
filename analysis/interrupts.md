# Interrupt inventory

Table `0x6438`, mtvec=`0x643b`, 104 pointer slots identified using the official CH32V303 D8 startup table.
7 implemented interrupt/exception entries: NMI is only `mret`; six others contain application logic.
78 vector-referenced self-loop handlers and 18 zero/reserved slots.
An additional 8 unreferenced weak self-loop stubs are retained in the code inventory.
A default handler does not demonstrate that its interrupt is enabled.

EXTI2, EXTI9_5 and EXTI15_10 handle buttons. TIM3 drives periodic power/battery work.
USART1 IDLE handles the 13-byte DMA snapshot; DMA1 channel 5 only clears its transfer-complete interrupt.
Routines end with `mret`, not normal C returns. Curated prototypes use `void(void)`.

| Slot | SDK identity | Target | Observed body |
|---:|---|---|---|
| 0 | _start | `0x6400` | implemented |
| 1 | 0 | `0x0` | reserved |
| 2 | NMI_Handler | `0xcb88` | implemented |
| 3 | HardFault_Handler | `0xcb8c` | default_self_loop |
| 4 | 0 | `0x0` | reserved |
| 5 | Ecall_M_Mode_Handler | `0xc5c2` | default_self_loop |
| 6 | 0 | `0x0` | reserved |
| 7 | 0 | `0x0` | reserved |
| 8 | Ecall_U_Mode_Handler | `0xc5c4` | default_self_loop |
| 9 | Break_Point_Handler | `0xc5c6` | default_self_loop |
| 10 | 0 | `0x0` | reserved |
| 11 | 0 | `0x0` | reserved |
| 12 | SysTick_Handler | `0xc5c8` | default_self_loop |
| 13 | 0 | `0x0` | reserved |
| 14 | SW_Handler | `0xc5ca` | default_self_loop |
| 15 | 0 | `0x0` | reserved |
| 16 | WWDG_IRQHandler | `0xc5cc` | default_self_loop |
| 17 | PVD_IRQHandler | `0xc5ce` | default_self_loop |
| 18 | TAMPER_IRQHandler | `0xc5d0` | default_self_loop |
| 19 | RTC_IRQHandler | `0xc5d2` | default_self_loop |
| 20 | FLASH_IRQHandler | `0xc5d4` | default_self_loop |
| 21 | RCC_IRQHandler | `0xc5d6` | default_self_loop |
| 22 | EXTI0_IRQHandler | `0xc5d8` | default_self_loop |
| 23 | EXTI1_IRQHandler | `0xc5da` | default_self_loop |
| 24 | EXTI2_IRQHandler | `0x7c0a` | implemented |
| 25 | EXTI3_IRQHandler | `0xc5de` | default_self_loop |
| 26 | EXTI4_IRQHandler | `0xc5e0` | default_self_loop |
| 27 | DMA1_Channel1_IRQHandler | `0xc5e2` | default_self_loop |
| 28 | DMA1_Channel2_IRQHandler | `0xc5e4` | default_self_loop |
| 29 | DMA1_Channel3_IRQHandler | `0xc5e6` | default_self_loop |
| 30 | DMA1_Channel4_IRQHandler | `0xc5e8` | default_self_loop |
| 31 | DMA1_Channel5_IRQHandler | `0xc570` | implemented |
| 32 | DMA1_Channel6_IRQHandler | `0xc5ec` | default_self_loop |
| 33 | DMA1_Channel7_IRQHandler | `0xc5ee` | default_self_loop |
| 34 | ADC1_2_IRQHandler | `0xc5f0` | default_self_loop |
| 35 | USB_HP_CAN1_TX_IRQHandler | `0xc5f2` | default_self_loop |
| 36 | USB_LP_CAN1_RX0_IRQHandler | `0xc5f4` | default_self_loop |
| 37 | CAN1_RX1_IRQHandler | `0xc5f6` | default_self_loop |
| 38 | CAN1_SCE_IRQHandler | `0xc5f8` | default_self_loop |
| 39 | EXTI9_5_IRQHandler | `0x7c42` | implemented |
| 40 | TIM1_BRK_IRQHandler | `0xc5fc` | default_self_loop |
| 41 | TIM1_UP_IRQHandler | `0xc5fe` | default_self_loop |
| 42 | TIM1_TRG_COM_IRQHandler | `0xc600` | default_self_loop |
| 43 | TIM1_CC_IRQHandler | `0xc602` | default_self_loop |
| 44 | TIM2_IRQHandler | `0xc604` | default_self_loop |
| 45 | TIM3_IRQHandler | `0x7780` | implemented |
| 46 | TIM4_IRQHandler | `0xc608` | default_self_loop |
| 47 | I2C1_EV_IRQHandler | `0xc60a` | default_self_loop |
| 48 | I2C1_ER_IRQHandler | `0xc60c` | default_self_loop |
| 49 | I2C2_EV_IRQHandler | `0xc60e` | default_self_loop |
| 50 | I2C2_ER_IRQHandler | `0xc610` | default_self_loop |
| 51 | SPI1_IRQHandler | `0xc612` | default_self_loop |
| 52 | SPI2_IRQHandler | `0xc614` | default_self_loop |
| 53 | USART1_IRQHandler | `0xc468` | implemented |
| 54 | USART2_IRQHandler | `0xc618` | default_self_loop |
| 55 | USART3_IRQHandler | `0xc61a` | default_self_loop |
| 56 | EXTI15_10_IRQHandler | `0x7cd8` | implemented |
| 57 | RTCAlarm_IRQHandler | `0xc61e` | default_self_loop |
| 58 | 0 | `0x0` | reserved |
| 59 | TIM8_BRK_IRQHandler | `0xc620` | default_self_loop |
| 60 | TIM8_UP_IRQHandler | `0xc622` | default_self_loop |
| 61 | TIM8_TRG_COM_IRQHandler | `0xc624` | default_self_loop |
| 62 | TIM8_CC_IRQHandler | `0xc626` | default_self_loop |
| 63 | RNG_IRQHandler | `0xc628` | default_self_loop |
| 64 | 0 | `0xc62a` | default_self_loop |
| 65 | SDIO_IRQHandler | `0xc62c` | default_self_loop |
| 66 | TIM5_IRQHandler | `0xc62e` | default_self_loop |
| 67 | SPI3_IRQHandler | `0xc630` | default_self_loop |
| 68 | UART4_IRQHandler | `0xc632` | default_self_loop |
| 69 | UART5_IRQHandler | `0xc634` | default_self_loop |
| 70 | TIM6_IRQHandler | `0xc636` | default_self_loop |
| 71 | TIM7_IRQHandler | `0xc638` | default_self_loop |
| 72 | DMA2_Channel1_IRQHandler | `0xc63a` | default_self_loop |
| 73 | DMA2_Channel2_IRQHandler | `0xc63c` | default_self_loop |
| 74 | DMA2_Channel3_IRQHandler | `0xc63e` | default_self_loop |
| 75 | DMA2_Channel4_IRQHandler | `0xc640` | default_self_loop |
| 76 | DMA2_Channel5_IRQHandler | `0xc642` | default_self_loop |
| 77 | 0 | `0x0` | reserved |
| 78 | 0 | `0x0` | reserved |
| 79 | 0 | `0x0` | reserved |
| 80 | 0 | `0x0` | reserved |
| 81 | 0 | `0x0` | reserved |
| 82 | 0 | `0x0` | reserved |
| 83 | USBFS_IRQHandler | `0xc644` | default_self_loop |
| 84 | 0 | `0x0` | reserved |
| 85 | 0 | `0x0` | reserved |
| 86 | 0 | `0x0` | reserved |
| 87 | UART6_IRQHandler | `0xc646` | default_self_loop |
| 88 | UART7_IRQHandler | `0xc648` | default_self_loop |
| 89 | UART8_IRQHandler | `0xc64a` | default_self_loop |
| 90 | TIM9_BRK_IRQHandler | `0xc64c` | default_self_loop |
| 91 | TIM9_UP_IRQHandler | `0xc64e` | default_self_loop |
| 92 | TIM9_TRG_COM_IRQHandler | `0xc650` | default_self_loop |
| 93 | TIM9_CC_IRQHandler | `0xc652` | default_self_loop |
| 94 | TIM10_BRK_IRQHandler | `0xc654` | default_self_loop |
| 95 | TIM10_UP_IRQHandler | `0xc656` | default_self_loop |
| 96 | TIM10_TRG_COM_IRQHandler | `0xc658` | default_self_loop |
| 97 | TIM10_CC_IRQHandler | `0xc65a` | default_self_loop |
| 98 | DMA2_Channel6_IRQHandler | `0xc65c` | default_self_loop |
| 99 | DMA2_Channel7_IRQHandler | `0xc65e` | default_self_loop |
| 100 | DMA2_Channel8_IRQHandler | `0xc660` | default_self_loop |
| 101 | DMA2_Channel9_IRQHandler | `0xc662` | default_self_loop |
| 102 | DMA2_Channel10_IRQHandler | `0xc664` | default_self_loop |
| 103 | DMA2_Channel11_IRQHandler | `0xc666` | default_self_loop |
