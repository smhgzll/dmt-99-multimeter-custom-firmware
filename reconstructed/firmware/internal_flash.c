/* Settings/history page at 0x08040000 (256 bytes): backend for the portable
 * storage/persistence.c contract of d928 (load) and d87c (commit).
 *
 * d87c sequence: HCLK=SYSCLK/2 (CFGR0 |= 0x80, required for program/erase above
 * 120 MHz), interrupts off (QingKe CSR 0x800 = 0x6000), FLASH_Unlock,
 * ClearFlag(EOP|WRPRTERR|BSY), fast unlock, 256-byte fast page erase, fast
 * lock, up to 128 FLASH_ProgramHalfWord calls while the status stays
 * FLASH_COMPLETE (4), FLASH_Lock. The original never restores HCLK or the
 * interrupt state because power-down halts afterwards; end() restores both so
 * the backend is also safe when the latch does not remove power.
 * The page is outside the firmware image (see dmt99.ld). An erased CH32V30x
 * halfword reads 0xe339, which the application treats as "not initialized"
 * (halfword 0 != 0) and answers with defaults plus language selection.
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "persistence.h"

static uint32_t saved_cfgr0;
static uint32_t saved_mstatus;
static bool unlocked;

static bool read_page(void *context,uint32_t address,uint16_t *out,uint16_t count)
{
    (void)context;
    if(address!=DMT_SETTINGS_ADDRESS || count>128)return false;
    const volatile uint16_t *page=(const volatile uint16_t *)address;
    for(uint16_t i=0;i<count;++i)out[i]=page[i];
    return true;
}
static bool begin(void *context)
{
    (void)context;
#if !DMT_FW_SETTINGS_WRITES
    return false;
#else
    __asm__ volatile("csrr %0, mstatus":"=r"(saved_mstatus));
    __asm__ volatile("csrc mstatus, 8");
    saved_cfgr0=RCC->CFGR0;
    RCC->CFGR0|=RCC_HPRE_DIV2;
    FLASH_Unlock();
    unlocked=true;
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPRTERR|FLASH_FLAG_BSY);
    return true;
#endif
}
static bool erase_page(void *context,uint32_t address)
{
    (void)context;
    if(!unlocked || address!=DMT_SETTINGS_ADDRESS)return false;
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(address);
    FLASH_Lock_Fast();
    return true;
}
static uint32_t program_halfword(void *context,uint32_t address,uint16_t value)
{
    (void)context;
    if(!unlocked || address<DMT_SETTINGS_ADDRESS || address>=DMT_SETTINGS_ADDRESS+DMT_SETTINGS_BYTES || (address&1))return FLASH_ERROR_PG;
    return (uint32_t)FLASH_ProgramHalfWord(address,value);
}
static void end(void *context)
{
    (void)context;
    if(!unlocked)return;
    FLASH_Lock();
    unlocked=false;
    RCC->CFGR0=(RCC->CFGR0&~(uint32_t)RCC_HPRE)|(saved_cfgr0&RCC_HPRE);
    if(saved_mstatus&8)__asm__ volatile("csrs mstatus, 8");
}
const dmt_settings_backend fw_settings_backend={0,read_page,begin,erase_page,program_halfword,end};
