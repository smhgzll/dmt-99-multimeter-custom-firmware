/* Clock tree for the DMT99 board (8 MHz crystal).
 *
 * SystemInit reproduces the original 0xd212, which is the WCH SDK
 * SetSysClockTo144_HSE path: HSE x18 PLL = 144 MHz, AHB /1, APB2 /1, APB1 /2,
 * HSE start-up budget 0x1000 polls. The original leaves the core on the 8 MHz
 * HSI when the crystal fails, which makes the fixed TIM3 prescaler run 18x slow.
 * This firmware instead falls back to the SDK's HSI x18 configuration, so every
 * peripheral keeps its nominal rate; fw_clock_source reports which path ran.
 * SystemCoreClockUpdate is the SDK D8 decoder (original 0xd2fc).
 */
#include "ch32v30x.h"
#include "firmware.h"

uint32_t SystemCoreClock=144000000;
volatile uint8_t fw_clock_source;
static const uint8_t ahb_shift[16]={0,0,0,0,0,0,0,0,1,2,3,4,6,7,8,9};

static void pll_select(void)
{
    RCC->CTLR|=RCC_PLLON;
    while(!(RCC->CTLR&RCC_PLLRDY)){}
    RCC->CFGR0&=~(uint32_t)RCC_SW;
    RCC->CFGR0|=RCC_SW_PLL;
    while((RCC->CFGR0&RCC_SWS)!=0x08){}
}
void SystemInit(void)
{
    /* Reset values exactly as the SDK/original prologue. */
    RCC->CTLR|=0x00000001;
    RCC->CFGR0&=0xF0FF0000;
    RCC->CTLR&=0xFEF6FFFF;
    RCC->CTLR&=0xFFFBFFFF;
    RCC->CFGR0&=0xFF80FFFF;
    RCC->INTR=0x009F0000;

    /* SetSysClockTo144_HSE, register write for register write. */
    RCC->CTLR|=RCC_HSEON;
    volatile uint32_t polls=0;
    while(!(RCC->CTLR&RCC_HSERDY) && ++polls!=HSE_STARTUP_TIMEOUT){}
    if(RCC->CTLR&RCC_HSERDY){
        RCC->CFGR0|=RCC_HPRE_DIV1;
        RCC->CFGR0|=RCC_PPRE2_DIV1;
        RCC->CFGR0|=RCC_PPRE1_DIV2;
        RCC->CFGR0&=~(uint32_t)(RCC_PLLSRC|RCC_PLLXTPRE|RCC_PLLMULL);
        RCC->CFGR0|=RCC_PLLSRC_HSE|RCC_PLLXTPRE_HSE|RCC_PLLMULL18;
        fw_clock_source=FW_CLOCK_HSE_PLL;
    }else{
        /* Not in the original: SDK SetSysClockTo144_HSI instead of staying on
         * the 8 MHz HSI with every fixed prescaler 18x off. */
        RCC->CTLR&=~(uint32_t)RCC_HSEON;
        EXTEN->EXTEN_CTR|=EXTEN_PLL_HSI_PRE;
        RCC->CFGR0|=RCC_HPRE_DIV1;
        RCC->CFGR0|=RCC_PPRE2_DIV1;
        RCC->CFGR0|=RCC_PPRE1_DIV2;
        RCC->CFGR0&=~(uint32_t)(RCC_PLLSRC|RCC_PLLXTPRE|RCC_PLLMULL);
        RCC->CFGR0|=RCC_PLLSRC_HSI_Div2|RCC_PLLMULL18;
        fw_clock_source=FW_CLOCK_HSI_PLL;
    }
    pll_select();
}
void SystemCoreClockUpdate(void)
{
    uint32_t cfgr=RCC->CFGR0,clock;
    switch(cfgr&RCC_SWS){
    case 0x04:clock=HSE_VALUE;break;
    case 0x08:{
        uint32_t mul=((cfgr&RCC_PLLMULL)>>18)+2;if(mul==17)mul=18;
        if(!(cfgr&RCC_PLLSRC))clock=(EXTEN->EXTEN_CTR&EXTEN_PLL_HSI_PRE)?HSI_VALUE*mul:(HSI_VALUE>>1)*mul;
        else clock=(cfgr&RCC_PLLXTPRE)?(HSE_VALUE>>1)*mul:HSE_VALUE*mul;
        break;
    }
    default:clock=HSI_VALUE;break;
    }
    SystemCoreClock=clock>>ahb_shift[(cfgr&RCC_HPRE)>>4];
}
