/* Device binding for authentic ChaN FatFs. Address evidence: dca8/dd00/dd48,
 * dd4e partition policy, fc9e/fcfc omitted CP936 table boundary. */
#include "filesystem.h"
#include "../vendor/fatfs/diskio.h"
#include <stddef.h>
#include "codepage.h"
static uint32_t codepage_fault;
uint32_t filesystem_codepage_fault(void){return codepage_fault;}
static dmt_filesystem_io io;
PARTITION VolToPart[FF_VOLUMES]={{0,1},{0,2}};
void filesystem_bind(const dmt_filesystem_io *p){io=p?*p:(dmt_filesystem_io){0};}
int filesystem_raw_read(uint32_t offset,void *out,uint32_t n){return io.read?io.read(io.context,offset,out,n):1;}
DSTATUS disk_status(BYTE p){return p<2 && io.read ? (io.write?0:STA_PROTECT):STA_NOINIT;}
DSTATUS disk_initialize(BYTE p){return disk_status(p);}
DRESULT disk_read(BYTE p,BYTE *b,LBA_t sector,UINT n){
 if(p>=2 || !n || sector>=2048 || n>2048-sector)return RES_PARERR;
 if(!io.read)return RES_NOTRDY;
 return io.read(io.context,sector<<12,b,n<<12)?RES_ERROR:RES_OK;
}
DRESULT disk_write(BYTE p,const BYTE *b,LBA_t sector,UINT n){
 if(p>=2 || !n || sector>=2048 || n>2048-sector)return RES_PARERR;
 if(!io.write)return RES_WRPRT;
 return io.write(io.context,sector<<12,b,n<<12)?RES_ERROR:RES_OK;
}
DRESULT disk_ioctl(BYTE p,BYTE cmd,void *b){
 if(p>=2)return RES_PARERR;
 if(cmd==GET_SECTOR_COUNT){if(!b)return RES_PARERR;*(LBA_t *)b=2048;}
 else if(cmd==GET_SECTOR_SIZE){if(!b)return RES_PARERR;*(WORD *)b=4096;}
 else if(cmd==GET_BLOCK_SIZE){if(!b)return RES_PARERR;*(DWORD *)b=1;}
 return RES_OK;
}
DWORD get_fattime(void){return 0x46210000;}
/* Preserve all defined original return paths; expose the actual failing ROM
 * address for all other non-ASCII conversions. FatFs sees an unmappable name. */
static WCHAR convert(DWORD c,WORD cp,int to_oem)
{
 dmt_codepage_result r=codepage_stock_convert(c,cp,to_oem);codepage_fault=r.fault_address;
 if(r.status==DMT_CODEPAGE_UNMAPPED_ROM && io.unresolved)io.unresolved(io.context,to_oem?0xfc9e:0xfcfc);
 return r.value;
}
WCHAR ff_uni2oem(DWORD c,WORD cp){return convert(c,cp,1);}
WCHAR ff_oem2uni(WCHAR c,WORD cp){return convert(c,cp,0);}
FRESULT filesystem_mount(FATFS *fs,unsigned volume){
 if(volume>1)return FR_INVALID_DRIVE;
 return f_mount(fs,volume?"1:":"0:",1);
}
FRESULT filesystem_original_partition_setup(void *work){
 const LBA_t map[4]={20,80,0,0};
 if(!io.write)return FR_WRITE_PROTECTED;
 return f_fdisk(0,map,work);
}
#if UINTPTR_MAX==UINT32_MAX
_Static_assert(sizeof(FIL)==4136,"original resource stack FIL");
_Static_assert(offsetof(FIL,fptr)==0x14,"original f_lseek ABI");
_Static_assert(offsetof(FATFS,win)==0x38,"original FAT window ABI");
_Static_assert(offsetof(FATFS,n_fatent)==0x1c,"original FAT cluster count ABI");
#endif
