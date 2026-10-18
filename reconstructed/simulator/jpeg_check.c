/* Host tool for DMT99 Studio: decodes one JPEG with the firmware's TJpgDec
 * build and the same 0x1c00-byte work pool as resource_draw_jpeg, so an
 * edited asset is checked against the device decoder rather than a desktop
 * library. Optionally writes the decoded RGB565 pixels (little-endian). */
#include "tjpgd.h"
#include "resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {const uint8_t *data;size_t size,at;uint16_t *pixels;unsigned width,height;} job;
static UINT input(JDEC *j,BYTE *buffer,UINT count)
{
 job *d=j->device;size_t n=d->size-d->at<count?d->size-d->at:count;
 if(buffer)memcpy(buffer,d->data+d->at,n);
 d->at+=n;return (UINT)n;
}
static UINT output(JDEC *j,void *bitmap,JRECT *rect)
{
 job *d=j->device;const WORD *p=bitmap;
 for(UINT y=rect->top;y<=rect->bottom;y++)for(UINT x=rect->left;x<=rect->right;x++,p++)
  if(d->pixels && x<d->width && y<d->height)d->pixels[y*d->width+x]=*p;
 return 1;
}
int main(int argc,char **argv)
{
 if(argc!=2 && argc!=3){fprintf(stderr,"usage: %s IMAGE.jpg [PIXELS.rgb565]\n",argv[0]);return 2;}
 FILE *f=fopen(argv[1],"rb");if(!f){perror(argv[1]);return 2;}
 fseek(f,0,SEEK_END);long size=ftell(f);fseek(f,0,SEEK_SET);
 uint8_t *data=size>0?malloc((size_t)size):0;
 if(!data || fread(data,1,(size_t)size,f)!=(size_t)size){fclose(f);fprintf(stderr,"read failed\n");return 2;}
 fclose(f);
 static union {uint32_t align;BYTE bytes[sizeof(((dmt_resources *)0)->pool.bytes)];} pool;
 JDEC decoder;job d={data,(size_t)size,0,0,0,0};
 JRESULT prepare=jd_prepare(&decoder,input,pool.bytes,sizeof pool.bytes,&d),decode=JDR_PAR;
 if(prepare==JDR_OK){
  d.width=decoder.width;d.height=decoder.height;d.pixels=calloc((size_t)d.width*d.height,2);
  decode=d.pixels?jd_decomp(&decoder,output,0):JDR_MEM1;
 }
 printf("{\"prepare\":%d,\"decode\":%d,\"width\":%u,\"height\":%u,\"pool\":%u,\"bytes\":%ld}\n",(int)prepare,(int)decode,d.width,d.height,(unsigned)sizeof pool.bytes,size);
 int rc=prepare==JDR_OK && decode==JDR_OK?0:1;
 if(argc==3 && !rc){
  FILE *o=fopen(argv[2],"wb");
  if(!o || fwrite(d.pixels,2,(size_t)d.width*d.height,o)!=(size_t)d.width*d.height)rc=2;
  if(o && fclose(o))rc=2;
 }
 free(d.pixels);free(data);return rc;
}
