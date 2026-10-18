#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void be32(unsigned char *p,uint32_t v)
{for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(24-i*8));}
static uint32_t crc32(const unsigned char *p,size_t n,uint32_t c)
{while(n--){c^=*p++;for(unsigned k=0;k<8;k++)c=c&1?(c>>1)^0xedb88320u:c>>1;}return c;}
static bool chunk(FILE *f,const char type[4],const unsigned char *p,uint32_t n)
{
 unsigned char word[4];be32(word,n);
 uint32_t crc=crc32((const unsigned char *)type,4,~0u);crc=~crc32(p,n,crc);
 if(fwrite(word,1,4,f)!=4 || fwrite(type,1,4,f)!=4 || (n && fwrite(p,1,n,f)!=n))return false;
 be32(word,crc);return fwrite(word,1,4,f)==4;
}
bool simulator_png(const char *path,const uint16_t *pixels,unsigned width,unsigned height)
{
 if(!path || !pixels || !width || !height || width>4096 || height>4096)return false;
 size_t path_length=strlen(path);
 if(path_length<4 || strcmp(path+path_length-4,".png"))return false;
 FILE *existing=fopen(path,"rb");
 if(existing){unsigned char signature[8];size_t got=fread(signature,1,8,existing);fclose(existing);
  static const unsigned char expected[]={137,80,78,71,13,10,26,10};
  if(got!=8 || memcmp(signature,expected,8))return false;
 }
 size_t row=1+width*3u,n=row*height,blocks=(n+65534)/65535;
 unsigned char *raw=malloc(n),*z=malloc(2+n+blocks*5+4);
 if(!raw || !z){free(raw);free(z);return false;}
 for(unsigned y=0;y<height;y++){
  raw[y*row]=0;
  for(unsigned x=0;x<width;x++){
   uint16_t c=pixels[y*width+x];unsigned r=c>>11,g=(c>>5)&63,b=c&31;
   size_t i=y*row+1+x*3;raw[i]=(r<<3)|(r>>2);raw[i+1]=(g<<2)|(g>>4);raw[i+2]=(b<<3)|(b>>2);
  }
 }
 size_t at=2;z[0]=0x78;z[1]=0x01;
 for(size_t p=0;p<n;){
  size_t len=n-p;if(len>65535)len=65535;
  z[at++]=p+len==n;z[at++]=(unsigned char)len;z[at++]=(unsigned char)(len>>8);
  z[at++]=(unsigned char)~len;z[at++]=(unsigned char)(~len>>8);
  for(size_t i=0;i<len;i++)z[at++]=raw[p+i];
  p+=len;
 }
 uint32_t a=1,b=0;for(size_t i=0;i<n;i++){a=(a+raw[i])%65521;b=(b+a)%65521;}be32(z+at,(b<<16)|a);at+=4;
 unsigned char header[13]={0};be32(header,width);be32(header+4,height);header[8]=8;header[9]=2;
 FILE *f=fopen(path,"wb");bool ok=false;
 if(f){static const unsigned char signature[]={137,80,78,71,13,10,26,10};
  ok=fwrite(signature,1,8,f)==8 && chunk(f,"IHDR",header,13) && chunk(f,"IDAT",z,(uint32_t)at) && chunk(f,"IEND",0,0);
  if(fclose(f))ok=false;
 }
 free(z);free(raw);return ok;
}
