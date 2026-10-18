#include "flash_export.h"

/* Placement experiment only. Objects are never linked to a device entry point.
 * RAM execution has no implied permission to read RDP-protected Flash.
 */
#ifdef DMT_EXPORT_RAM_EXPERIMENT
#define CODE __attribute__((section(".ramtext.dmt_export")))
#else
#define CODE
#endif
static CODE void le16(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static CODE void le32(uint8_t *p,uint32_t v)
{for(unsigned i=0;i<4;++i)p[i]=(uint8_t)(v>>(i*8));}
static CODE uint32_t crc_update(uint32_t c,const uint8_t *p,size_t n)
{
    for(size_t i=0;i<n;++i){c^=p[i];for(unsigned j=0;j<8;++j)c=(c>>1)^((0u-(c&1u))&UINT32_C(0xedb88320));}
    return c;
}
CODE uint32_t dmt_export_crc32(const uint8_t *p,size_t n)
{return crc_update(UINT32_MAX,p,n)^UINT32_MAX;}
CODE bool dmt_export_plan_valid(const dmt_export_plan *p)
{
    if(!p || !p->bytes || !p->allowed_bytes || p->passes<2 || p->passes>8)return false;
    if(p->allowed_start<DMT_EXPORT_MAIN_BASE || p->allowed_start>=DMT_EXPORT_MAIN_END)return false;
    if(p->allowed_bytes>DMT_EXPORT_MAIN_END-p->allowed_start)return false;
    if(p->start<p->allowed_start || p->start>=p->allowed_start+p->allowed_bytes)return false;
    return p->bytes<=p->allowed_start+p->allowed_bytes-p->start;
}
static CODE bool frame(const dmt_export_plan *p,const dmt_export_io *io,uint8_t *b,
                  unsigned type,unsigned pass,uint32_t index,uint32_t address,
                  uint16_t count,unsigned status)
{
    b[0]='D';b[1]='9';b[2]='9';b[3]='X';b[4]=1;b[5]=(uint8_t)type;
    b[6]=(uint8_t)pass;b[7]=p->passes;le32(b+8,p->session);le32(b+12,index);
    le32(b+16,address);le32(b+20,p->start);le32(b+24,p->bytes);
    le16(b+28,count);b[30]=(uint8_t)status;b[31]=0;
    le32(b+DMT_EXPORT_HEADER+count,dmt_export_crc32(b,DMT_EXPORT_HEADER+count));
    return io->send(io->context,b,DMT_EXPORT_HEADER+count+4u);
}
CODE bool dmt_flash_export(const dmt_export_plan *p,const dmt_export_io *io)
{
    uint8_t b[DMT_EXPORT_MAX_FRAME],again[DMT_EXPORT_BLOCK];
    if(!dmt_export_plan_valid(p) || !io || !io->read || !io->send)return false;
    for(unsigned pass=0;pass<p->passes;++pass){
        uint32_t offset=0,index=0,crc=UINT32_MAX;
        while(offset<p->bytes){
            uint16_t n=(uint16_t)((p->bytes-offset)>DMT_EXPORT_BLOCK?DMT_EXPORT_BLOCK:p->bytes-offset);
            unsigned error=0;
            if(!io->read(io->context,p->start+offset,b+DMT_EXPORT_HEADER,n) ||
               !io->read(io->context,p->start+offset,again,n))error=DMT_EXPORT_READ_FAILED;
            if(!error)for(unsigned i=0;i<n;++i)if(b[DMT_EXPORT_HEADER+i]!=again[i]){error=DMT_EXPORT_UNSTABLE;break;}
            if(error){(void)frame(p,io,b,DMT_EXPORT_ERROR,pass,index,p->start+offset,0,error);return false;}
            crc=crc_update(crc,b+DMT_EXPORT_HEADER,n);
            if(!frame(p,io,b,DMT_EXPORT_DATA,pass,index,p->start+offset,n,0))return false;
            offset+=n;++index;
        }
        le32(b+DMT_EXPORT_HEADER,crc^UINT32_MAX);
        if(!frame(p,io,b,DMT_EXPORT_END,pass,index,p->start+p->bytes,4,0))return false;
    }
    return true;
}
