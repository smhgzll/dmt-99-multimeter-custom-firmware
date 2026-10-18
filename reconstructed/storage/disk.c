/* 0xdca8,0xdcd0,0xdd00. Both drive IDs map to the same SPI address space.
 * The original length is narrowed to 16 bits: count 16 becomes zero bytes.
 * Preserve the ABI for comparison; callers should issue <=15 sectors/call.
 */
#include "disk.h"
int disk_read_recovered(const dmt_disk *d,uint8_t drive,uint8_t *out,uint32_t sector,uint32_t count)
{if(drive>=2)return 4;spi_flash_read(d->bus,out,sector<<12,(uint16_t)(count<<12));return 0;}
int disk_write_recovered(const dmt_disk *d,uint8_t drive,const uint8_t *p,uint32_t sector,uint32_t count)
{if(drive>=2 || !count)return 4;return spi_flash_write_preserve_sector(d->bus,p,sector<<12,(uint16_t)(count<<12),d->sector_scratch,d->poll_budget)?0:1;}
int disk_ioctl_recovered(uint8_t drive,uint8_t command,void *p)
{
    if(drive>=2)return 4;
    if(command==1)*(uint32_t *)p=2048;
    else if(command==2)*(uint16_t *)p=4096;
    else if(command==3)*(uint32_t *)p=1;
    return 0;
}
