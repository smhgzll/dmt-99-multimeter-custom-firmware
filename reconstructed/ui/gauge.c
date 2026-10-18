#include "gauge.h"
#include <string.h>
#include "bitmap_images.h"
extern const uint8_t dmt_scene_prefix[0x254c8-0x247c0],dmt_scene_suffix[0x27194-0x26338];
const uint8_t *scene_data(uint32_t a,size_t n)
{
 if(a>=0x247c0 && a<0x254c8 && n<=0x254c8-a)return dmt_scene_prefix+(a-0x247c0);
 if(a>=0x254c8 && a<0x26338 && n<=0x26338-a)return (const uint8_t *)&numeric_bitmap_glyphs+(a-0x254c8);
 if(a>=0x26338 && a<0x27194 && n<=0x27194-a)return dmt_scene_suffix+(a-0x26338);
 return 0;
}
const uint8_t *scene_bitmap(uint32_t a,size_t *length)
{
 const uint8_t *p=scene_data(a,6);if(!p)return 0;
 unsigned w=p[2]|p[3]<<8,h=p[4]|p[5]<<8;
 *length=6+(size_t)((w+7)/8)*h;return scene_data(a,*length);
}
void gauge_bitmap(const dmt_view *v,const uint8_t *p,size_t n,uint32_t rom,uint16_t x,uint16_t y,uint16_t fg,uint16_t bg,float value)
{
 uint32_t bits;memcpy(&bits,&value,4);uint32_t args[8]={rom,x,y,fg,bg,bits};view_trace(v,0x1a468,args,0);
 if(n<6 || !v || !v->status || !v->status->canvas)return;
 int16_t base=(int16_t)(y+3);float threshold;
 if(value>99){threshold=0;base=0;}else if(value==0)threshold=205.f;else threshold=(float)((double)(100.f-value)*2.01);
 unsigned width=p[2]|p[3]<<8,height=p[4]|p[5]<<8,stride=(width+7)/8;
 if(width>=2048 || height>255 || n<6+(size_t)stride*height)return;
 const dmt_canvas *c=v->status->canvas;float limit=(float)base+threshold;
 for(unsigned r=0;r<height;r++)for(unsigned col=0;col<width;col++)if(p[6+r*stride+col/8]&(128>>(col&7))){
  int16_t row=(int16_t)(y+r);c->pixel(c->context,(uint16_t)(x+col),(uint16_t)row,(float)row<limit?bg:fg);
 }
}

void gauge_lower(const dmt_view *v,unsigned n){
 static const uint16_t shape[][6]={
{0,21,213,25,213,4},
{0,25,210,28,210,4},
{1,26,212,26,212,4},
{1,27,211,27,211,4},
{0,28,207,31,207,5},
{1,29,209,29,209,5},
{1,30,208,30,208,5},
{0,31,204,34,204,3},
{1,32,206,32,206,3},
{1,33,205,33,205,3},
{0,33,201,36,201,2},
{1,35,203,35,203,2},
{1,36,202,36,202,2},
{0,36,198,36,200,1},
{0,295,213,299,213,4},
{0,292,210,295,210,4},
{1,294,212,294,212,4},
{1,293,211,293,211,4},
{0,289,207,292,207,5},
{1,291,209,291,209,5},
{1,290,208,290,208,5},
{0,286,204,289,204,3},
{1,288,206,288,206,3},
{1,287,205,287,205,3},
{0,284,201,287,201,2},
{1,285,203,285,203,2},
{1,284,202,284,202,2},
{0,284,198,284,200,1}};
 for(unsigned i=0;i<sizeof shape/sizeof shape[0];i++){const uint16_t *p=shape[i];unsigned rank=6-p[5];if(p[5]==4)rank=1;else if(p[5]==5)rank=2;uint16_t c=n>5?0:n>=rank?0xfd40:0x4228;
 if(v && v->status && v->status->canvas){if(p[0])v->status->canvas->pixel(v->status->canvas->context,p[1],p[2],c);else graphics_line(v->status->canvas,p[1],p[2],p[3],p[4],c);}
 }
}

void gauge_upper(const dmt_view *v,unsigned n){
 static const uint16_t shape[][6]={
{0,30,37,36,37,5},
{1,36,38,36,38,5},
{0,32,33,35,33,5},
{0,36,34,36,36,5},
{0,30,30,33,30,4},
{1,34,31,34,31,4},
{1,35,32,35,32,4},
{0,28,27,30,27,3},
{1,31,28,31,28,3},
{1,32,29,32,29,3},
{0,25,24,27,24,2},
{1,28,25,28,25,2},
{1,29,26,29,26,2},
{0,20,21,24,21,1},
{1,25,22,25,22,1},
{1,26,23,26,23,1},
{0,284,38,289,38,5},
{0,285,34,288,34,5},
{0,284,35,284,37,5},
{0,287,31,290,31,4},
{1,286,32,286,32,4},
{1,285,33,285,33,4},
{0,290,28,292,28,3},
{1,289,29,289,29,3},
{1,288,30,288,30,3},
{0,293,25,295,25,2},
{1,292,26,292,26,2},
{1,291,27,291,27,2},
{0,296,22,301,22,1},
{1,295,23,295,23,1},
{1,294,24,294,24,1}};
 for(unsigned i=0;i<sizeof shape/sizeof shape[0];i++){const uint16_t *p=shape[i];unsigned rank=6-p[5];uint16_t c=n>5?0:n>=rank?0xfd40:0x4228;
 if(v && v->status && v->status->canvas){if(p[0])v->status->canvas->pixel(v->status->canvas->context,p[1],p[2],c);else graphics_line(v->status->canvas,p[1],p[2],p[3],p[4],c);}
 }
}

static void label(const dmt_view *v,uint16_t x,uint16_t y,uint32_t n,uint8_t digits,uint16_t color)
{uint32_t a[8]={x,y,n,digits,12,0,color};view_trace(v,0x84da,a,0);if(v && v->status && v->status->canvas)graphics_number(v->status->canvas,x,y,n,digits,12,0,color);}
static void packed(const dmt_view *v,uint32_t address,uint16_t x,uint16_t y,uint16_t color)
{size_t n;const uint8_t *p=scene_bitmap(address,&n);uint32_t a[8]={address,x,y,0,color};view_trace(v,0x1a360,a,0);if(p && v && v->status && v->status->canvas)graphics_bitmap(v->status->canvas,x,y,p,n,0,color);}
void gauge_legacy(const dmt_view *v,uint32_t value)
{
 uint32_t args[8]={value};view_trace(v,0x1066c,args,0);
 if(!v || !v->status)return;
 unsigned half=value>>1,n=half&65535;uint16_t c=half?0xfd40:0x4228;
 if(half<5){label(v,306,208,0,1,c);label(v,11,208,0,1,c);gauge_lower(v,half);}
 else{gauge_lower(v,5);label(v,306,208,0,1,0xfd40);label(v,11,208,0,1,0xfd40);n++;}
 if(half>=5 && n>49){gauge_upper(v,5);label(v,295,36,90,2,0xfd40);label(v,299,24,100,3,0xfd40);label(v,16,36,90,2,0xfd40);label(v,2,24,100,3,0xfd40);n=45;}
 else if(n<45){gauge_upper(v,0);label(v,295,36,90,2,0x4228);label(v,299,24,100,3,0x4228);label(v,16,36,90,2,0x4228);label(v,2,24,100,3,0x4228);}
 else{if(n>=46 && n<=49)gauge_upper(v,n-45);label(v,295,36,90,2,0xfd40);label(v,16,36,90,2,0xfd40);if(n>45)n=45;}
 unsigned digit=10;
 for(unsigned i=0;i<40;i++){
  uint16_t color=(int32_t)i<(int32_t)(n-5)?0xfd40:0x4228;uint16_t y=(uint16_t)(195-i*4);
  if(i%5){packed(v,0x25120,284,y,color);packed(v,0x25108,33,y,color);}
  else{packed(v,0x25114,284,y,color);packed(v,0x250fc,30,y,color);label(v,294,(uint16_t)(192-i*4),digit,2,color);label(v,16,(uint16_t)(192-i*4),digit,2,color);digit+=10;}
 }
}
