#include "view.h"
#include "numeric_view.h"
#include "chinese.h"
#include "gauge.h"
void view_trace(const dmt_view *v,uint32_t a,const uint32_t p[8],const char *s){if(v && v->trace)v->trace(v->context,a,p,s);}
void view_image(const dmt_view *v,const char *s,uint16_t x,uint16_t y)
{uint32_t a[8]={0,x,y};view_trace(v,0x678c,a,s);if(v && v->status && v->status->image)v->status->image(v->status->context,s,x,y);}
void view_fill(const dmt_view *v,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t c)
{uint32_t a[8]={x,y,x1,y1,c};view_trace(v,0x8a02,a,0);if(v && v->status && v->status->canvas)graphics_fill(v->status->canvas,x,y,x1,y1,c);}
void view_text(const dmt_view *v,uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint8_t size,uint16_t c,const char *s)
{uint32_t a[8]={x,y,w,h,size,c};view_trace(v,0x85cc,a,s);if(v && v->status && v->status->canvas)graphics_text_box(v->status->canvas,x,y,w,h,size,c,s,true);}
void view_numeric(const dmt_view *v,const char *s,uint8_t view,uint8_t mode,uint8_t unit,uint8_t layout)
{uint32_t a[8]={0};view_trace(v,layout?0x109e4:0x10bd0,a,s);if(v && v->status){if(layout)numeric_jpeg_text(v->status,s,view,mode,unit);else numeric_bitmap_text(v->status,s,view,mode,unit);}}
void view_chinese(const dmt_view *v,uint16_t x,uint16_t y,const uint8_t *s,uint8_t size,uint16_t color)
{
 uint32_t a[8]={x,y,0,color,color,size,1};view_trace(v,0x8e46,a,(const char *)s);
 if(!v || !v->status || !v->status->canvas)return;
 const dmt_canvas *c=v->status->canvas;unsigned count,stride,offset;
 switch(size){case 12:count=12;stride=26;offset=0;break;case 16:count=63;stride=34;offset=312;break;case 24:count=49;stride=74;offset=2456;break;case 32:count=1;stride=130;offset=6084;break;default:return;}
 for(;s[0] && s[1];s+=2,x=(uint16_t)(x+size))for(unsigned n=0;n<count;n++){
  const uint8_t *p=dmt_chinese_rom+offset+n*stride;if(p[0]!=s[0] || p[1]!=s[1])continue;
  unsigned row_bytes=(size+7)/8;
  for(unsigned yy=0;yy<size;yy++)for(unsigned xx=0;xx<size;xx++)if(p[2+yy*row_bytes+xx/8]&(1u<<(xx&7)))c->pixel(c->context,(uint16_t)(x+xx),(uint16_t)(y+yy),color);
 }
}

void view_ascii(const dmt_view *v,uint16_t x,uint16_t y,const char *s,uint16_t fg,uint16_t bg,uint8_t size)
{uint32_t a[8]={x,y,0,fg,bg,size,1};view_trace(v,0x8588,a,s);if(v && v->status && v->status->canvas)graphics_ascii_text(v->status->canvas,x,y,s,size,fg,bg,true);}
void view_line(const dmt_view *v,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t c)
{uint32_t a[8]={x,y,x1,y1,c};view_trace(v,0x8050,a,0);if(v && v->status && v->status->canvas)graphics_line(v->status->canvas,x,y,x1,y1,c);}
void view_border(const dmt_view *v,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t c)
{uint32_t a[8]={x,y,x1,y1,c};view_trace(v,0x8194,a,0);if(v && v->status && v->status->canvas)graphics_rectangle(v->status->canvas,x,y,x1,y1,c);}
void view_pixel(const dmt_view *v,uint16_t x,uint16_t y,uint16_t c)
{uint32_t a[8]={x,y,c};view_trace(v,0x7f00,a,0);if(v && v->status && v->status->canvas)v->status->canvas->pixel(v->status->canvas->context,x,y,c);}
void view_measurement_char(const dmt_view *v,uint16_t x,uint16_t y,uint8_t ch,uint8_t size,uint16_t color)
{uint32_t a[8]={x,y,ch,size,color,1};view_trace(v,0x8266,a,0);if(v && v->status && v->status->canvas)graphics_measurement_char(v->status->canvas,x,y,(char)ch,size,color,true);}
void view_cut_fill(const dmt_view *v,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint8_t cut,uint16_t c)
{uint32_t a[8]={x,y,x1,y1,cut,c};view_trace(v,0x80d4,a,0);if(v && v->status && v->status->canvas)graphics_cut_fill(v->status->canvas,x,y,x1,y1,cut,c);}
void view_cut_border(const dmt_view *v,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint8_t cut,uint16_t c)
{uint32_t a[8]={x,y,x1,y1,cut,c};view_trace(v,0x81ce,a,0);if(v && v->status && v->status->canvas)graphics_cut_border(v->status->canvas,x,y,x1,y1,cut,c);}
void view_ascii_char(const dmt_view *v,uint16_t x,uint16_t y,uint8_t ch,uint8_t size,uint16_t c)
{uint32_t a[8]={x,y,ch,size,c,c,1};view_trace(v,0x83d6,a,0);if(v && v->status && v->status->canvas)graphics_ascii(v->status->canvas,x,y,ch,size,c,c,true);}
void view_bitmap(const dmt_view *v,uint32_t rom,uint16_t x,uint16_t y,uint16_t fg,uint16_t bg)
{uint32_t a[8]={rom,x,y,fg,bg};view_trace(v,0x1a360,a,0);size_t n;const uint8_t *p=scene_bitmap(rom,&n);if(p && v && v->status && v->status->canvas)graphics_bitmap(v->status->canvas,x,y,p,n,fg,bg);}
