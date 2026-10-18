#include "flash_export.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned reads,sends,fail_read,fail_send;bool unstable;uint32_t min,max;} mock;
static bool read_mock(void *ctx,uint32_t address,uint8_t *out,size_t n)
{
    mock *m=ctx;++m->reads;
    assert(address>=m->min && address<=m->max && n<=m->max-address);
    if(m->fail_read==m->reads)return false;
    for(size_t i=0;i<n;++i)out[i]=(uint8_t)(address+i);
    if(m->unstable && (m->reads%2)==0)out[0]^=1;
    return true;
}
static bool send_mock(void *ctx,const uint8_t *b,size_t n)
{
    mock *m=ctx;++m->sends;assert(n>=36 && n<=DMT_EXPORT_MAX_FRAME);
    assert(memcmp(b,"D99X",4)==0);
    uint32_t c=0;for(unsigned i=0;i<4;++i)c|=(uint32_t)b[n-4+i]<<(8*i);
    assert(dmt_export_crc32(b,n-4)==c);
    if(m->unstable || m->fail_read)assert(b[5]==DMT_EXPORT_ERROR);
    return m->fail_send!=m->sends;
}
int main(void)
{
    assert(dmt_export_crc32((const uint8_t *)"123456789",9)==UINT32_C(0xcbf43926));
    assert(dmt_export_crc32(NULL,0)==0);
    dmt_export_plan p={DMT_EXPORT_MAIN_BASE,0x78000,DMT_EXPORT_MAIN_BASE,257,0x12345678,2};
    mock m={0};m.min=p.start;m.max=p.start+p.bytes;dmt_export_io io={&m,read_mock,send_mock};
    assert(dmt_flash_export(&p,&io));assert(m.reads==8 && m.sends==6);
    /* Every invalid plan must stop before any callback. Overflow is subtraction-checked. */
    const dmt_export_plan bad[]={
        {0,0x78000,0,256,1,2},{DMT_EXPORT_MAIN_BASE,0xffffffff,DMT_EXPORT_MAIN_BASE,1,1,2},
        {DMT_EXPORT_MAIN_BASE,0x78000,DMT_EXPORT_MAIN_END-1,2,1,2},
        {DMT_EXPORT_MAIN_BASE,0x78000,0xfffffff0,64,1,2},
        {DMT_EXPORT_MAIN_BASE,256,DMT_EXPORT_MAIN_BASE,257,1,2},
        {DMT_EXPORT_MAIN_BASE,256,DMT_EXPORT_MAIN_BASE,0,1,2},
        {DMT_EXPORT_MAIN_BASE,256,DMT_EXPORT_MAIN_BASE,1,1,1},
        {DMT_EXPORT_MAIN_BASE,256,DMT_EXPORT_MAIN_BASE,1,1,9},
        {DMT_EXPORT_MAIN_END,1,DMT_EXPORT_MAIN_END,1,1,2}
    };
    for(size_t i=0;i<sizeof(bad)/sizeof(*bad);++i){m.reads=m.sends=0;assert(!dmt_flash_export(&bad[i],&io));assert(!m.reads && !m.sends);}
    p.start=DMT_EXPORT_MAIN_END-1;p.bytes=1;m.min=p.start;m.max=p.start+1;
    assert(dmt_flash_export(&p,&io));
    p.start=DMT_EXPORT_MAIN_BASE;p.bytes=257;m.min=p.start;m.max=p.start+p.bytes;
    m.reads=m.sends=0;m.fail_read=1;assert(!dmt_flash_export(&p,&io));assert(m.reads==1 && m.sends==1);
    m.reads=m.sends=0;m.fail_read=2;assert(!dmt_flash_export(&p,&io));assert(m.reads==2 && m.sends==1);
    m.reads=m.sends=m.fail_read=0;m.unstable=true;assert(!dmt_flash_export(&p,&io));assert(m.reads==2 && m.sends==1);
    m.reads=m.sends=0;m.unstable=false;m.fail_send=1;assert(!dmt_flash_export(&p,&io));assert(m.reads==2 && m.sends==1);
    assert(!dmt_flash_export(NULL,&io));assert(!dmt_flash_export(&p,NULL));
    io.read=NULL;assert(!dmt_flash_export(&p,&io));
    puts("flash export: CRC, policy/boundaries, repeated reads, read/send errors passed");
    return 0;
}
