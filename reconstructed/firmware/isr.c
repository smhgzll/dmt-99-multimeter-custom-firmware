/* Interrupt handlers: c468 USART1, c570 DMA1 channel 5, 7780 TIM3, 7c0a
 * EXTI2, 7c42 EXTI9_5, 7cd8 EXTI15_10.
 *
 * Interrupts do not nest (INTSYSCR=0, see startup.S); each handler is a plain
 * GCC machine-mode interrupt function. Key debouncing (including the PC8
 * long-press poll of up to 610 ms) stays inside the EXTI handlers as in the
 * original. The original TIM3 handler samples the ADC for 250 ms, may draw the
 * low-battery countdown and may run power-down inside the interrupt, and the
 * PC2 handler also runs power-down inside the interrupt. Those actions share
 * the display, FatFs and Flash with the foreground, so here the handlers only
 * record the request and the foreground performs the original sequence. If a
 * power-key request is not serviced within 3 s the TIM3 handler cuts power.
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "runtime.h"

#define IRQ __attribute__((interrupt("machine")))

#if DMT_FW_BRINGUP
static void key_log(const char *line,uint8_t before)
{
    if(fw_app.control.event!=before)
        fw_printf("[%lu] key %s -> event %u (run %u view %u)\r\n",(unsigned long)fw_millis(),line,
            fw_app.control.event,fw_app.run,fw_app.control.view);
}
#define KEY_LOG(line,before) key_log(line,before)
#else
#define KEY_LOG(line,before) ((void)(before))
#endif

void IRQ USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_IDLE)==RESET)return;
    DMA_Cmd(DMA1_Channel5,DISABLE);
    USART_ClearFlag(USART1,USART_FLAG_IDLE);
    (void)USART_ReceiveData(USART1);
    /* Snapshot while the channel is stopped, then rearm as the original. */
    const uint8_t *b=fw_dma_frame;
    for(unsigned i=0;i<13;++i)fw_rx.frame[i]=b[i];
    fw_rx.rx_suppressed=measurement_frame_is_blank(b);
    fw_rx.frame_ready=1;fw_rx.frames++;
    if(b[2]==0xfd){for(unsigned i=0;i<13;++i)fw_rx.saved_event[i]=b[i];fw_rx.event_pending=1;fw_rx.events++;}
    DMA_SetCurrDataCounter(DMA1_Channel5,13);
    DMA_Cmd(DMA1_Channel5,ENABLE);
}
void IRQ DMA1_Channel5_IRQHandler(void){DMA_ClearITPendingBit(DMA1_IT_TC5);}
void IRQ TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==RESET)return;
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    if(fw_timer_pending<255)fw_timer_pending++;else fw_counters.timer_ticks_lost++;
#if DMT_THEMES
    /* Not while a theme is written or deleted: cutting power mid-way could
     * damage the FAT; the foreground powers down when the operation ends. */
    if(fw_shutdown_request && !fw_theme_busy && ++fw_shutdown_age>3)fw_emergency_power_off();
#else
    if(fw_shutdown_request && ++fw_shutdown_age>3)fw_emergency_power_off();
#endif
}
void IRQ EXTI2_IRQHandler(void)
{
    uint8_t before=fw_app.control.event;
    buttons_exti_2(&fw_button_io,&fw_app.control.event,fw_normal_operation());
    /* 7c0a does not return from power_down, so event 8 is only posted when
     * the key did not request power-down. */
    if(fw_shutdown_request)fw_app.control.event=before;
    KEY_LOG(fw_shutdown_request?"PC2 power-down request":"PC2",before);
}
void IRQ EXTI9_5_IRQHandler(void)
{
    uint16_t pending=(uint16_t)(EXTI->INTFR&0x300);uint8_t before=fw_app.control.event;
    buttons_exti_9_5(&fw_button_io,pending,&fw_app.control.event,&fw_app.power.idle_seconds);
    KEY_LOG(pending&0x100?"PC8":"PC9",before);
}
void IRQ EXTI15_10_IRQHandler(void)
{
    uint16_t pending=(uint16_t)(EXTI->INTFR&0x1c00);uint8_t before=fw_app.control.event;
    buttons_exti_15_10(&fw_button_io,pending,&fw_app.control.event,&fw_app.power.idle_seconds);
    KEY_LOG(pending&0x400?"PC10":pending&0x800?"PC11":"PC12",before);
}
