/* NVIC_PriorityGroupConfig 0xcb8e and NVIC_Init 0xcb94 of the V2.4 image.
 *
 * The image links an older WCH SDK whose NVIC_Init encodes the IPRIOR byte
 * differently from the pinned SDK used for the other peripheral drivers, so
 * the original encoding is reproduced here. Group 2 (used by main 0x908a):
 *   preemption <= 1: byte = (4*preemption + sub) << 4
 *   preemption  > 1: byte = ((4*preemption + 0xf8 + sub) & 0xff) << 4 | 0x80
 * giving EXTI and DMA1_CH5 0x50, EXTI9_5 after PC8 0x60, USART1 0xa0 and
 * TIM3 0xf0. The enable/disable write to IENR/IRER follows. The firmware does
 * not enable PFIC nesting, so the bytes only order simultaneous requests.
 */
#include "wch_platform.h"
static uint32_t priority_group;
void recovered_nvic_priority_group(uint32_t group){priority_group=group;}
uint8_t recovered_nvic_priority_byte(uint32_t group,uint8_t preemption,uint8_t sub)
{
    uint32_t value;
    switch(group){
    case 0:value=(uint32_t)sub<<4;break;
    case 1:value=(uint32_t)sub<<4;if(preemption==1)value|=0x80;break;
    case 2:
        if(preemption<=1)value=((uint32_t)preemption*4+sub)<<4;
        else value=(((uint32_t)preemption*4+0xf8+sub)&0xff)<<4|0x80;
        break;
    case 3:
        if(preemption<=3)value=((uint32_t)preemption*2+sub)<<4;
        else value=(((uint32_t)preemption*2+0xf8+sub)&0xff)<<4|0x80;
        break;
    case 4:value=(uint32_t)preemption<<4;break;
    default:return 0xff;   /* original stores nothing for other groups */
    }
    return (uint8_t)value;
}
void recovered_nvic_init(const NVIC_InitTypeDef *init)
{
    uint8_t channel=init->NVIC_IRQChannel;
    if(priority_group<=4)
        ((volatile uint8_t *)&PFIC->IPRIOR[0])[channel]=recovered_nvic_priority_byte(priority_group,
            init->NVIC_IRQChannelPreemptionPriority,init->NVIC_IRQChannelSubPriority);
    if(init->NVIC_IRQChannelCmd!=DISABLE)PFIC->IENR[channel>>5]=1u<<(channel&31);
    else PFIC->IRER[channel>>5]=1u<<(channel&31);
}
