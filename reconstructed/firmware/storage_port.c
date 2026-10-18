/* External W25Q64JV on SPI1 (PA4 CS, PA5 SCK, PA6 MISO, PA7 MOSI; d38a) and
 * the resource start-up of dd4e.
 *
 * d442: one byte exchange with TXE/RXNE polls bounded at 201 each; a timeout
 * returns 0 exactly as the original.
 * dd4e order: SPI init and JEDEC probe (d86c/d782), f_fdisk(0,{20,80},work),
 * f_mount("0:") then f_setlabel("0:BOOT") when it mounted, f_mount("1:") into
 * the same FATFS object. Sharing the object is what lets the original open
 * unprefixed resource paths ("menu/...") on the second partition.
 *
 * Write policy: the original rewrites the MBR sector and the volume-0 root
 * directory sector on every boot (erase + program). Here each 4 KiB sector that
 * FatFs writes is first compared with the chip; identical sectors are skipped,
 * erased sectors are programmed directly and other sectors are erased and
 * programmed. The resulting chip contents equal the original's; only redundant
 * erase cycles disappear. DMT_FW_SPI_FLASH_WRITES=0 refuses every write that
 * would change the chip.
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "storage_port.h"
#include "spi_flash.h"
#include "wch_platform.h"

static void select(void *context,bool active)
{(void)context;if(active)GPIOA->BCR=GPIO_Pin_4;else GPIOA->BSHR=GPIO_Pin_4;}
static uint8_t transfer(void *context,uint8_t value)
{
    (void)context;
    uint8_t polls=201;
    while(!(SPI1->STATR&SPI_I2S_FLAG_TXE)){if(!--polls)return 0;}
    SPI1->DATAR=value;
    polls=201;
    while(!(SPI1->STATR&SPI_I2S_FLAG_RXNE)){if(!--polls)return 0;}
    return (uint8_t)SPI1->DATAR;
}
const dmt_spi_bus fw_spi_bus={0,select,transfer};
fw_storage_status fw_storage;

enum {SECTOR=4096,BUSY_POLLS=4000000};
static int read_bytes(void *context,uint32_t offset,void *out,uint32_t bytes)
{
    (void)context;
    uint8_t *p=out;
    while(bytes){
        uint16_t chunk=bytes>0x8000?0x8000:(uint16_t)bytes;
        spi_flash_read(&fw_spi_bus,p,offset,chunk);
        p+=chunk;offset+=chunk;bytes-=chunk;
    }
    return 0;
}
/* Compare one 4 KiB sector with the chip. Returns 0 equal, 1 differs with the
 * chip sector erased, 2 differs with programmed bytes present. */
static int compare_sector(uint32_t offset,const uint8_t *p)
{
    uint8_t buffer[64];bool differs=false,erased=true;
    for(uint32_t at=0;at<SECTOR;at+=sizeof buffer){
        spi_flash_read(&fw_spi_bus,buffer,offset+at,sizeof buffer);
        for(unsigned i=0;i<sizeof buffer;++i){
            if(buffer[i]!=p[at+i])differs=true;
            if(buffer[i]!=0xff)erased=false;
        }
    }
    return !differs?0:erased?1:2;
}
static int write_bytes(void *context,uint32_t offset,const void *data,uint32_t bytes)
{
    (void)context;
    const uint8_t *p=data;
    if((offset|bytes)&(SECTOR-1))return 1;
    for(;bytes;bytes-=SECTOR,offset+=SECTOR,p+=SECTOR){
        int state=compare_sector(offset,p);
        if(!state){fw_storage.writes_skipped++;continue;}
#if !DMT_FW_SPI_FLASH_WRITES
        fw_storage.writes_refused++;
        return 1;
#else
        if(state==2 && !spi_flash_erase_sector(&fw_spi_bus,offset>>12,BUSY_POLLS)){fw_storage.write_errors++;return 1;}
        if(!spi_flash_program_pages(&fw_spi_bus,p,offset,SECTOR,BUSY_POLLS)){fw_storage.write_errors++;return 1;}
        if(compare_sector(offset,p)){fw_storage.write_errors++;return 1;}
        fw_storage.sectors_written++;
#endif
    }
    return 0;
}
static void unresolved(void *context,uint32_t address)
{(void)context;fw_storage.codepage_faults++;fw_storage.last_codepage_entry=address;}

void fw_spi_flash_init(void)
{
    recovered_spi1_init();
    fw_storage.jedec=spi_flash_read_jedec_id(&fw_spi_bus);
    fw_storage.capacity=spi_flash_capacity_from_jedec(fw_storage.jedec);
}
void fw_storage_bind(void)
{
    static const dmt_filesystem_io io={0,read_bytes,write_bytes,unresolved};
    filesystem_bind(&io);
}
void fw_resources_init(FATFS *fs,void *work)
{
    static const LBA_t partitions[4]={20,80,0,0};
    fw_spi_flash_init();
    fw_storage_bind();
    fw_storage.fdisk=f_fdisk(0,partitions,work);
    fw_storage.mount0=filesystem_mount(fs,0);
    if(fw_storage.mount0==FR_OK)fw_storage.label=f_setlabel("0:BOOT");
    fw_storage.mount1=filesystem_mount(fs,1);
}
