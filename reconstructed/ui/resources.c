/* Native application adapters at6708,6734,678c; decoder is authentic ChaN. */
#include "resources.h"
#include <stddef.h>
static UINT input(JDEC *j,BYTE *buffer,UINT count)
{
 dmt_resources *r=j->device;
 if(!buffer){r->file_result=f_lseek(&r->file,f_tell(&r->file)+count);return r->file_result?0:count;}
 UINT got=0;r->file_result=f_read(&r->file,buffer,count,&got);
 /* Original6708 returns the FatFs error number if read fails. */
 return r->file_result?(UINT)r->file_result:got;
}
static UINT output(JDEC *j,void *bitmap,JRECT *rect)
{
 dmt_resources *r=j->device;const WORD *p=bitmap;
 if(r->display){
  display_window(r->display,(uint16_t)(r->x+rect->left),(uint16_t)(r->y+rect->top),
   (uint16_t)(rect->right-rect->left+1),(uint16_t)(rect->bottom-rect->top+1));
  display_write_command(&r->display->bus,r->display->pixels_command);
 }
 for(UINT y=rect->top;y<=rect->bottom;y++)for(UINT x=rect->left;x<=rect->right;x++){
  if(r->display)display_write_data(&r->display->bus,*p);
  else if(r->canvas && r->canvas->pixel)r->canvas->pixel(r->canvas->context,(uint16_t)(r->x+x),(uint16_t)(r->y+y),*p);
  p++;r->pixels_emitted++;
 }
 return 1;
}
int resource_draw_jpeg(dmt_resources *r,const char *path,uint16_t x,uint16_t y)
{
 r->file_result=f_open(&r->file,path,FA_READ);
 r->prepare_result=JDR_PAR;r->decode_result=JDR_PAR;
 if(r->file_result!=FR_OK)return 1;
 r->images_opened++;
 r->prepare_result=jd_prepare(&r->decoder,input,r->pool.bytes,sizeof(r->pool.bytes),r);
 if(r->prepare_result==JDR_OK){r->x=x;r->y=y;r->decode_result=jd_decomp(&r->decoder,output,0);}
 r->file_result=f_close(&r->file);return 0;
}
bool resource_view_image(void *r,const char *path,uint16_t x,uint16_t y){return resource_draw_jpeg(r,path,x,y)==0;}
#if UINTPTR_MAX==UINT32_MAX
_Static_assert(sizeof(JDEC)==124,"original decoder object");
_Static_assert(offsetof(JDEC,pool)==0x6c,"original JPEG pool ABI");
_Static_assert(offsetof(JDEC,device)==0x78,"original JPEG callback ABI");
#endif
