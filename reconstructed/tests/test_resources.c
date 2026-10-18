/* Real acquired image is read-only; write tests use a private heap copy. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include "resources.h"
#include "resource_volume.h"
#include "numeric_view.h"
#include "bitmap_images.h"
#include "../vendor/fatfs/diskio.h"
static unsigned char *image;
static unsigned writes,gaps;
static int fail_reads;
static uint32_t pixels,hash;
static int rd(void *c,uint32_t p,void *b,uint32_t n){(void)c;if(fail_reads || p>8388608 || n>8388608-p)return 1;memcpy(b,image+p,n);return 0;}
static int wr(void *c,uint32_t p,const void *b,uint32_t n){(void)c;if(p>8388608 || n>8388608-p)return 1;memcpy(image+p,b,n);writes++;return 0;}
static void gap(void *c,uint32_t a){(void)c;assert(a==0xfc9e || a==0xfcfc);gaps++;}
static void pixel(void *c,uint16_t x,uint16_t y,uint16_t v){(void)c;hash=(hash^x)*16777619u;hash=(hash^y)*16777619u;hash=(hash^v)*16777619u;pixels++;}
static unsigned char *load(const char *p,size_t *size){FILE *f=fopen(p,"rb");assert(f);assert(!fseek(f,0,SEEK_END));long n=ftell(f);assert(n>=0);rewind(f);unsigned char *b=malloc((size_t)n+1);assert(b);assert(fread(b,1,(size_t)n,f)==(size_t)n);fclose(f);*size=(size_t)n;return b;}
int main(int argc,char **argv){
 assert(argc==3);size_t size;image=load(argv[1],&size);assert(size==8388608);
 dmt_filesystem_io io={0,rd,0,gap};filesystem_bind(&io);FATFS fs={0};
 assert(filesystem_mount(&fs,1)==FR_OK);assert(fs.fs_type==1 && fs.ssize==4096 && fs.database==470);
 assert(filesystem_original_partition_setup(fs.win)==FR_WRITE_PROTECTED);assert(!writes);
 FILE *list=fopen(argv[2],"r");assert(list);char path[512];unsigned count=0,jpegs=0;
 static dmt_resources r;dmt_canvas canvas={0,pixel,65535,65535,0};r.canvas=&canvas;
 while(fgets(path,sizeof(path),list)){
  path[strcspn(path,"\r\n")]=0;char native[600],extracted[800];snprintf(native,sizeof(native),"1:/%s",path);snprintf(extracted,sizeof(extracted),"../extracted_flash/volume1/%s",path);
  size_t n;unsigned char *expected=load(extracted,&n);FIL f;assert(f_open(&f,native,FA_READ)==FR_OK);
  unsigned char actual[4096];size_t at=0;
  while(at<n){UINT got=0;assert(f_read(&f,actual,sizeof(actual),&got)==FR_OK);assert(got && got<=n-at);assert(!memcmp(actual,expected+at,got));at+=got;}
  assert(f_lseek(&f,1)==FR_OK);UINT got;assert(f_read(&f,actual,7,&got)==FR_OK);assert(got==(n>8?7:n-1));assert(!memcmp(actual,expected+1,got));assert(f_close(&f)==FR_OK);free(expected);count++;
  if(strstr(path,".JPG") || strstr(path,".jpg")){
   pixels=0;hash=2166136261u;assert(resource_draw_jpeg(&r,native,3,7)==0);assert(r.prepare_result==0 && r.decode_result==0);assert(pixels==r.decoder.width*r.decoder.height);uint32_t first=hash;
   pixels=0;hash=2166136261u;assert(resource_draw_jpeg(&r,native,3,7)==0);assert(hash==first);jpegs++;
  }
 }
 fclose(list);assert(count==298 && jpegs==296);assert(!writes);
 assert(resource_draw_jpeg(&r,"1:/missing.jpg",0,0)==1);
 /* Original ignores decoder failure after a successful open. */
 assert(resource_draw_jpeg(&r,"1:/System Volume Information/WPSettings.dat",0,0)==0);assert(r.prepare_result!=JDR_OK);
 assert(ff_uni2oem(0x4e2d,936)==0 && gaps>0);
 FRESULT last;assert(resource_optional_logo(&r,&fs,&last));assert(last==FR_OK);
 resource_remove_logo(&fs,&last);assert(last==FR_OK && !writes);
 dmt_status_view view={&r,resource_view_image,0,&canvas};
 pixels=0;numeric_jpeg_text(&view," 12.34",1,0x0c,6);assert(pixels && r.decode_result==JDR_OK);
 pixels=0;numeric_bitmap_text(&view,"-1.25",3,0x12,21);assert(pixels && r.decode_result==JDR_OK);
 uint32_t before=pixels;graphics_bitmap(&canvas,0,0,numeric_bitmap_glyphs[0],5,1,0);assert(pixels==before);
 /* Disposable image write/MBR tests; no original FILE handle exists now. */
 io.write=wr;filesystem_bind(&io);assert(filesystem_original_partition_setup(fs.win)==FR_OK);assert(writes==1);
 assert(image[446+4]==7 && image[462+4]==7 && image[510]==0x55 && image[511]==0xaa);
 /* Native read failure and a corrupt chain, only in the disposable copy. */
 assert(filesystem_mount(&fs,1)==FR_OK);FIL bad;assert(f_open(&bad,"1:/on.jpg",FA_READ)==FR_OK);
 DWORD cluster=bad.obj.sclust;assert(f_close(&bad)==FR_OK);
 for(unsigned fat=0;fat<2;fat++){
  uint32_t p=(464+fat)*4096+cluster+cluster/2;
  if(cluster&1){image[p]&=15;image[p+1]=0;}else{image[p]=0;image[p+1]&=240;}
 }
 assert(filesystem_mount(&fs,1)==FR_OK);assert(f_open(&bad,"1:/on.jpg",FA_READ)==FR_OK);
 unsigned char block[8192];UINT got;assert(f_read(&bad,block,sizeof(block),&got)==FR_INT_ERR);assert(got==4096);assert(f_close(&bad)==FR_OK);
 fail_reads=1;assert(filesystem_mount(&fs,1)==FR_DISK_ERR);fail_reads=0;
 assert(f_mount(0,"1:",0)==FR_OK);filesystem_bind(0);free(image);
 puts("6 resource/filesystem tests passed (298 files,296 JPEGs twice; one CP936 name excluded explicitly)");return 0;
}
