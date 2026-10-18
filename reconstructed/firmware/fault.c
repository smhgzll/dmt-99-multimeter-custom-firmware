/* Trap handling and boot diagnostics.
 *
 * The original routes every unused vector and all exceptions to self-loops
 * (0xc5c2.., HardFault 0xcb8c), freezing the meter. Here the trap cause, PC,
 * trap value and stack pointer are kept in .noinit and the MCU is reset; the
 * next boot prints the record on the debug console (PA2, 115200). The reset
 * flags of RCC_RSTSCKR are reported and cleared once per boot.
 */
#include "ch32v30x.h"
#include "firmware.h"

#define FAULT_MAGIC 0x46415554u
extern uint32_t _susrstack[],_eusrstack[];
__attribute__((section(".noinit"))) static fw_fault_record fault;

void fw_system_reset(void)
{
    NVIC->CFGR=NVIC_KEY3|(1u<<7);
    for(;;){}
}
__attribute__((noreturn,used)) void fw_unexpected_trap(void)
{
    uint32_t mcause,mepc,mtval,sp;
    __asm__ volatile("csrr %0, mcause":"=r"(mcause));
    __asm__ volatile("csrr %0, mepc":"=r"(mepc));
    __asm__ volatile("csrr %0, mtval":"=r"(mtval));
    __asm__ volatile("mv %0, sp":"=r"(sp));
    uint32_t count=fault.magic==FAULT_MAGIC?fault.count+1:1;
    fault=(fw_fault_record){FAULT_MAGIC,mcause,mepc,mtval,sp,count,0};
    fw_printf("\r\n*** trap mcause=%08lx mepc=%08lx mtval=%08lx sp=%08lx, resetting\r\n",
        (unsigned long)mcause,(unsigned long)mepc,(unsigned long)mtval,(unsigned long)sp);
    fw_delay_us(2000);
    fw_system_reset();
}
uint32_t fw_stack_unused(void)
{
    const uint32_t *p=_susrstack;
    while(p<_eusrstack && *p==0x5354414bu)p++;
    return (uint32_t)((const uint8_t *)p-(const uint8_t *)_susrstack);
}
void fw_fault_boot_report(void)
{
    uint32_t flags=RCC->RSTSCKR;
    RCC->RSTSCKR|=RCC_RMVF;
    fw_printf("\r\n%s\r\nclock=%lu Hz source=%s reset=%08lx\r\n",fw_version_text,(unsigned long)SystemCoreClock,
        fw_clock_source==FW_CLOCK_HSE_PLL?"HSE":fw_clock_source==FW_CLOCK_HSI_PLL?"HSI-fallback":"?",(unsigned long)flags);
    fw_printf("option USER=%02lx RDPR=%s\r\n",(unsigned long)((FLASH->OBR>>2)&0xff),(FLASH->OBR&2)?"protected":"open");
    if(fault.magic==FAULT_MAGIC){
        fw_printf("previous trap #%lu mcause=%08lx mepc=%08lx mtval=%08lx sp=%08lx\r\n",(unsigned long)fault.count,
            (unsigned long)fault.mcause,(unsigned long)fault.mepc,(unsigned long)fault.mtval,(unsigned long)fault.sp);
        fault.magic=0;
    }
}
