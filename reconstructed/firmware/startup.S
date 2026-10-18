/* CH32V303RCT6 startup for the reconstructed DMT99 firmware.
 *
 * Two layouts are built from this file: the standalone image at 0x00000000
 * (dmt99.ld) and, with DMT_FW_APP_IMAGE=1, the USB update image at 0x6400
 * behind the manufacturer bootloader (dmt99-app.ld), whose header and vector
 * table position match the V2.4 update file. The text below describes the
 * standalone layout.
 *
 * Layout follows the official WCH startup_ch32v30x_D8.S: a 4-byte jump at
 * 0x00000000 (alias of 0x08000000), then a 104-slot table of absolute handler
 * addresses selected by mtvec mode 3. The slot list matches the vector table of
 * the V2.4 application at 0x6438 (analysis/interrupts.md).
 *
 * Deliberate differences from the original handle_reset (0xc668):
 *  - INTSYSCR (0x804) = 0: no hardware prologue (HPE) and no PFIC nesting. The
 *    handlers are ordinary GCC "interrupt" functions, so their register saving
 *    is fully software-defined and verifiable in the disassembly.
 *  - mstatus.MPP = machine: main runs in M-mode instead of WCH's U-mode default.
 *  - mstatus.FS = off: the image is built soft-float (the original also used
 *    software floating point routines).
 * CORECFGR (0xbc0) keeps the SDK/original value 0x1f.
 */
	.section .init,"ax",@progbits
	.global _start
	.option push
	.option norvc
_start:
	j handle_reset
#if DMT_FW_APP_IMAGE
	/* USB update image (dmt99-app.ld): the V2.4 update file header, 12 nops
	 * and an ebreak, so the vector table lands at +0x38 as in V2.4. */
	.rept 12
	.word 0x00000013
	.endr
	.word 0x00100073
#endif
	.option pop

	.section .vector,"ax",@progbits
	.align 2
	.global _vector_base
	.option push
	.option norvc
_vector_base:
	.word _start                    /* 0 */
	.word 0
	.word NMI_Handler
	.word HardFault_Handler
	.word 0
	.word Ecall_M_Mode_Handler
	.word 0
	.word 0
	.word Ecall_U_Mode_Handler
	.word Break_Point_Handler
	.word 0
	.word 0
	.word SysTick_Handler
	.word 0
	.word SW_Handler
	.word 0
	.word WWDG_IRQHandler           /* 16 */
	.word PVD_IRQHandler
	.word TAMPER_IRQHandler
	.word RTC_IRQHandler
	.word FLASH_IRQHandler
	.word RCC_IRQHandler
	.word EXTI0_IRQHandler
	.word EXTI1_IRQHandler
	.word EXTI2_IRQHandler          /* 24: power key PC2 */
	.word EXTI3_IRQHandler
	.word EXTI4_IRQHandler
	.word DMA1_Channel1_IRQHandler
	.word DMA1_Channel2_IRQHandler
	.word DMA1_Channel3_IRQHandler
	.word DMA1_Channel4_IRQHandler
	.word DMA1_Channel5_IRQHandler  /* 31: USART1 RX DMA */
	.word DMA1_Channel6_IRQHandler
	.word DMA1_Channel7_IRQHandler
	.word ADC1_2_IRQHandler
	.word USB_HP_CAN1_TX_IRQHandler
	.word USB_LP_CAN1_RX0_IRQHandler
	.word CAN1_RX1_IRQHandler
	.word CAN1_SCE_IRQHandler
	.word EXTI9_5_IRQHandler        /* 39: keys PC8/PC9 */
	.word TIM1_BRK_IRQHandler
	.word TIM1_UP_IRQHandler
	.word TIM1_TRG_COM_IRQHandler
	.word TIM1_CC_IRQHandler
	.word TIM2_IRQHandler
	.word TIM3_IRQHandler           /* 45: 1 s power/battery tick */
	.word TIM4_IRQHandler
	.word I2C1_EV_IRQHandler
	.word I2C1_ER_IRQHandler
	.word I2C2_EV_IRQHandler
	.word I2C2_ER_IRQHandler
	.word SPI1_IRQHandler
	.word SPI2_IRQHandler
	.word USART1_IRQHandler         /* 53: measurement UART IDLE */
	.word USART2_IRQHandler
	.word USART3_IRQHandler
	.word EXTI15_10_IRQHandler      /* 56: keys PC10/PC11/PC12 */
	.word RTCAlarm_IRQHandler
	.word 0
	.word TIM8_BRK_IRQHandler
	.word TIM8_UP_IRQHandler
	.word TIM8_TRG_COM_IRQHandler
	.word TIM8_CC_IRQHandler
	.word RNG_IRQHandler
	.word 0
	.word SDIO_IRQHandler
	.word TIM5_IRQHandler
	.word SPI3_IRQHandler
	.word UART4_IRQHandler
	.word UART5_IRQHandler
	.word TIM6_IRQHandler
	.word TIM7_IRQHandler
	.word DMA2_Channel1_IRQHandler
	.word DMA2_Channel2_IRQHandler
	.word DMA2_Channel3_IRQHandler
	.word DMA2_Channel4_IRQHandler
	.word DMA2_Channel5_IRQHandler
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word USBFS_IRQHandler          /* 83 */
	.word 0
	.word 0
	.word 0
	.word UART6_IRQHandler
	.word UART7_IRQHandler
	.word UART8_IRQHandler
	.word TIM9_BRK_IRQHandler
	.word TIM9_UP_IRQHandler
	.word TIM9_TRG_COM_IRQHandler
	.word TIM9_CC_IRQHandler
	.word TIM10_BRK_IRQHandler
	.word TIM10_UP_IRQHandler
	.word TIM10_TRG_COM_IRQHandler
	.word TIM10_CC_IRQHandler
	.word DMA2_Channel6_IRQHandler
	.word DMA2_Channel7_IRQHandler
	.word DMA2_Channel8_IRQHandler
	.word DMA2_Channel9_IRQHandler
	.word DMA2_Channel10_IRQHandler
	.word DMA2_Channel11_IRQHandler /* 103 */
	.option pop

/* Every slot not implemented by the firmware resolves to fw_default_handler,
 * which enters fw_unexpected_trap: it records the trap and resets
 * (firmware/fault.c). The original's equivalents are self-loops. */
	.section .text.fw_default_handler,"ax",@progbits
	.align 1
fw_default_handler:
	j fw_unexpected_trap

	.macro weak_handler name
	.weak \name
	.set \name, fw_default_handler
	.endm
	weak_handler HardFault_Handler
	weak_handler Ecall_M_Mode_Handler
	weak_handler Ecall_U_Mode_Handler
	weak_handler Break_Point_Handler
	weak_handler SysTick_Handler
	weak_handler SW_Handler
	weak_handler WWDG_IRQHandler
	weak_handler PVD_IRQHandler
	weak_handler TAMPER_IRQHandler
	weak_handler RTC_IRQHandler
	weak_handler FLASH_IRQHandler
	weak_handler RCC_IRQHandler
	weak_handler EXTI0_IRQHandler
	weak_handler EXTI1_IRQHandler
	weak_handler EXTI2_IRQHandler
	weak_handler EXTI3_IRQHandler
	weak_handler EXTI4_IRQHandler
	weak_handler DMA1_Channel1_IRQHandler
	weak_handler DMA1_Channel2_IRQHandler
	weak_handler DMA1_Channel3_IRQHandler
	weak_handler DMA1_Channel4_IRQHandler
	weak_handler DMA1_Channel5_IRQHandler
	weak_handler DMA1_Channel6_IRQHandler
	weak_handler DMA1_Channel7_IRQHandler
	weak_handler ADC1_2_IRQHandler
	weak_handler USB_HP_CAN1_TX_IRQHandler
	weak_handler USB_LP_CAN1_RX0_IRQHandler
	weak_handler CAN1_RX1_IRQHandler
	weak_handler CAN1_SCE_IRQHandler
	weak_handler EXTI9_5_IRQHandler
	weak_handler TIM1_BRK_IRQHandler
	weak_handler TIM1_UP_IRQHandler
	weak_handler TIM1_TRG_COM_IRQHandler
	weak_handler TIM1_CC_IRQHandler
	weak_handler TIM2_IRQHandler
	weak_handler TIM3_IRQHandler
	weak_handler TIM4_IRQHandler
	weak_handler I2C1_EV_IRQHandler
	weak_handler I2C1_ER_IRQHandler
	weak_handler I2C2_EV_IRQHandler
	weak_handler I2C2_ER_IRQHandler
	weak_handler SPI1_IRQHandler
	weak_handler SPI2_IRQHandler
	weak_handler USART1_IRQHandler
	weak_handler USART2_IRQHandler
	weak_handler USART3_IRQHandler
	weak_handler EXTI15_10_IRQHandler
	weak_handler RTCAlarm_IRQHandler
	weak_handler TIM8_BRK_IRQHandler
	weak_handler TIM8_UP_IRQHandler
	weak_handler TIM8_TRG_COM_IRQHandler
	weak_handler TIM8_CC_IRQHandler
	weak_handler RNG_IRQHandler
	weak_handler SDIO_IRQHandler
	weak_handler TIM5_IRQHandler
	weak_handler SPI3_IRQHandler
	weak_handler UART4_IRQHandler
	weak_handler UART5_IRQHandler
	weak_handler TIM6_IRQHandler
	weak_handler TIM7_IRQHandler
	weak_handler DMA2_Channel1_IRQHandler
	weak_handler DMA2_Channel2_IRQHandler
	weak_handler DMA2_Channel3_IRQHandler
	weak_handler DMA2_Channel4_IRQHandler
	weak_handler DMA2_Channel5_IRQHandler
	weak_handler USBFS_IRQHandler
	weak_handler UART6_IRQHandler
	weak_handler UART7_IRQHandler
	weak_handler UART8_IRQHandler
	weak_handler TIM9_BRK_IRQHandler
	weak_handler TIM9_UP_IRQHandler
	weak_handler TIM9_TRG_COM_IRQHandler
	weak_handler TIM9_CC_IRQHandler
	weak_handler TIM10_BRK_IRQHandler
	weak_handler TIM10_UP_IRQHandler
	weak_handler TIM10_TRG_COM_IRQHandler
	weak_handler TIM10_CC_IRQHandler
	weak_handler DMA2_Channel6_IRQHandler
	weak_handler DMA2_Channel7_IRQHandler
	weak_handler DMA2_Channel8_IRQHandler
	weak_handler DMA2_Channel9_IRQHandler
	weak_handler DMA2_Channel10_IRQHandler
	weak_handler DMA2_Channel11_IRQHandler

/* Original NMI_Handler (0xcb88) is a bare mret. */
	.section .text.NMI_Handler,"ax",@progbits
	.weak NMI_Handler
	.align 1
NMI_Handler:
	mret

	.section .text.handle_reset,"ax",@progbits
	.global handle_reset
	.align 1
handle_reset:
#if DMT_FW_APP_IMAGE
	/* Entered from the manufacturer bootloader rather than from reset: mask
	 * interrupts and drop whatever it may have left configured (PFIC enables,
	 * pending bits, fast-vector (VTF) slots, SysTick), so no stale interrupt
	 * reaches this image before main sets up its own sources. */
	csrci mstatus, 8
	li t0, 0xe000e000
	li t1, -1
	.irp off, 0x180, 0x184, 0x188, 0x18c, 0x280, 0x284, 0x288, 0x28c
	sw t1, \off(t0)
	.endr
	.irp off, 0x60, 0x64, 0x68, 0x6c
	sw zero, \off(t0)
	.endr
	li t0, 0xe000f000
	sw zero, 0(t0)
	sw zero, 4(t0)
#endif
	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop
	la sp, _eusrstack
	/* Copy initialized data. */
	la a0, _data_lma
	la a1, _data_vma
	la a2, _edata
	bgeu a1, a2, 2f
1:	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:	/* Clear .bss. .noinit is preserved across warm resets. */
	la a0, _sbss
	la a1, _ebss
	bgeu a0, a1, 2f
1:	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:	/* Paint the stack for the high-water report (fw_stack_unused). */
	la a0, _susrstack
	la a1, _eusrstack
	addi a1, a1, -64
	li t1, 0x5354414b
	bgeu a0, a1, 2f
1:	sw t1, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:	li t0, 0x1f
	csrw 0xbc0, t0
	csrw 0x804, zero
	li t0, 0x1880
	csrw mstatus, t0
	la t0, _vector_base
	ori t0, t0, 3
	csrw mtvec, t0
	call SystemInit
	la t0, main
	csrw mepc, t0
	mret
