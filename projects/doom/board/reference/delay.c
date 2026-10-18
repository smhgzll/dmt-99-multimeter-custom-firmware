/* Delay service replacing 1a626/1a64c.
 *
 * The original reprograms SysTick for every delay_ms call, waits for the
 * compare flag and stops the counter. Because button ISRs call delay_ms(20)
 * while the foreground may already be waiting, an interrupted foreground delay
 * ends early (the ISR leaves the flag set). Here SysTick counts freely at
 * HCLK/8 (18 MHz at 144 MHz) and a delay only compares elapsed counts, so the
 * requested duration is a lower bound in both foreground and ISR context.
 * The counting unit is the original's: SystemCoreClock/8000000 ticks per us.
 */
#include "ch32v30x.h"
#include "firmware.h"

static uint32_t ticks_per_us=18;
void fw_delay_init(void)
{
    ticks_per_us=SystemCoreClock/8000000u;
    if(!ticks_per_us)ticks_per_us=1;
    SysTick->CTLR=0;
    SysTick->SR=0;
    SysTick->CNT=0;
    SysTick->CMP=0xffffffffffffffffull;
    /* STE=1 enable, STCLK=0 HCLK/8, STIE=0, STRE=0 free running, MODE=up. */
    SysTick->CTLR=1;
}
uint64_t fw_ticks(void)
{
    /* 64-bit counter read as two halves; retry if the low word wrapped. */
    volatile uint32_t *cnt=(volatile uint32_t *)&SysTick->CNT;
    uint32_t hi,lo;
    do{hi=cnt[1];lo=cnt[0];}while(hi!=cnt[1]);
    return (uint64_t)hi<<32|lo;
}
uint32_t fw_millis(void){return (uint32_t)(fw_ticks()/(ticks_per_us*1000u));}
void fw_delay_us(uint32_t us)
{
    uint64_t start=fw_ticks(),span=(uint64_t)us*ticks_per_us;
    while(fw_ticks()-start<span){}
}
void fw_delay_ms(uint32_t ms)
{
    uint64_t start=fw_ticks(),span=(uint64_t)ms*ticks_per_us*1000u;
    while(fw_ticks()-start<span){}
}
