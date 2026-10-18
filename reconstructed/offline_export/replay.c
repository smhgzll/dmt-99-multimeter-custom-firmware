/* File-backed demonstration only; never dereferences an MCU address. */
#include "flash_export.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct {FILE *image;uint32_t base,bytes;} replay;
static bool read_image(void *ctx,uint32_t address,uint8_t *out,size_t n)
{
    replay *r=ctx;
    if(address<r->base || address-r->base>r->bytes || n>r->bytes-(address-r->base))return false;
    return fseek(r->image,(long)(address-r->base),SEEK_SET)==0 && fread(out,1,n,r->image)==n;
}
static bool send_stdout(void *ctx,const uint8_t *p,size_t n)
{(void)ctx;return fwrite(p,1,n,stdout)==n;}
static bool number(const char *s,uint32_t *out)
{
    char *end;errno=0;unsigned long v=strtoul(s,&end,0);
    if(errno || !*s || *s=='-' || *end || v>UINT32_MAX)return false;
    *out=(uint32_t)v;return true;
}
int main(int argc,char **argv)
{
    if(argc!=6){fprintf(stderr,"usage: %s IMAGE IMAGE_BASE START BYTES SESSION > capture.d99x\n",argv[0]);return 2;}
    replay r={0};dmt_export_plan p={0};
    if(!number(argv[2],&r.base) || !number(argv[3],&p.start) || !number(argv[4],&p.bytes) || !number(argv[5],&p.session))return 2;
    r.image=fopen(argv[1],"rb");if(!r.image){perror("image");return 2;}
    if(fseek(r.image,0,SEEK_END)!=0){fclose(r.image);return 2;}
    long size=ftell(r.image);if(size<=0 || (unsigned long)size>UINT32_MAX){fclose(r.image);return 2;}
    r.bytes=(uint32_t)size;p.allowed_start=r.base;p.allowed_bytes=r.bytes;p.passes=2;
    dmt_export_io io={&r,read_image,send_stdout};
    bool ok=dmt_flash_export(&p,&io);
    if(fflush(stdout)!=0)ok=false;
    fclose(r.image);return ok?0:1;
}
