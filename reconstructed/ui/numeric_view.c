/* Original109e4. The uint8 index wrap is bounded to256 characters here.
 * Valid original domain: a NUL-terminated measurement string within256 bytes. */
#include "numeric_view.h"
void numeric_jpeg_text(const dmt_status_view *v,const char *text,uint8_t view,uint8_t mode,uint8_t unit)
{
 uint16_t x=view==3?46:43,y=view==3?120:90;
 static const char *digits[]={"num/0.jpg","num/1.jpg","num/2.jpg","num/3.jpg","num/4.jpg","num/5.jpg","num/6.jpg","num/7.jpg","num/8.jpg","num/9.jpg"};
 for(unsigned i=0;i<256;i++){
  unsigned char c=(unsigned char)text[i];const char *path=0;uint16_t px=x;
  if(!c){
   if(mode==0x12 || ((mode==0x16 || mode==0x11) && unit==8))
    graphics_fill(v->canvas,(uint16_t)(x-7),y,(uint16_t)(x-1),(uint16_t)(y+51),0);
   return;
  }
  if(c>='0' && c<='9')path=digits[c-'0'];
  else switch(c){
   case '-':path="num/minus.jpg";break;
   case '.':path="num/radix.jpg";px=(uint16_t)(x-7);break;
   case 'O':path=digits[0];break;
   case 'A':path="num/a.jpg";break;
   case 'E':path="num/e.jpg";break;
   case 'L':path="num/l.jpg";break;
   case 'T':path="num/t.jpg";break;
   case 'U':path="num/u.jpg";break;
   default:graphics_fill(v->canvas,x,y,(uint16_t)(x+33),(uint16_t)(y+51),0);break;
  }
  if(path && v->image)v->image(v->context,path,px,y);
  if(c!='.' && i && text[i-1]!='.')graphics_fill(v->canvas,(uint16_t)(x-7),y,(uint16_t)(x-1),(uint16_t)(y+51),0);
  x=(uint16_t)(x+(c=='.'?7:40));
 }
}

/* Alternate numeric path10bd0. E and decimal remain JPEG resources. */
#include "bitmap_images.h"
void numeric_bitmap_text(const dmt_status_view *v,const char *text,uint8_t view,uint8_t mode,uint8_t unit)
{
 uint16_t x=view==3?35:30,y=view==3?130:90;
 for(unsigned i=0;i<256;i++){
  unsigned char c=(unsigned char)text[i];int glyph=-1;
  if(!c){
   if(mode==0x12 || ((mode==0x16 || mode==0x11) && unit==8))
    graphics_fill(v->canvas,(uint16_t)(x-5),y,(uint16_t)(x+3),(uint16_t)(y+51),0);
   return;
  }
  if(c>='0' && c<='9')glyph=c-'0';
  else switch(c){case 'O':glyph=0;break;case 'A':glyph=10;break;case 'L':glyph=11;break;case 'T':glyph=12;break;case 'U':glyph=13;break;default:break;}
  if(glyph>=0)graphics_bitmap(v->canvas,x,y,numeric_bitmap_glyphs[glyph],264,0xfd40,0);
  else if(c=='.' || c=='E'){
   if(v->image)v->image(v->context,c=='.'?"num/radix.jpg":"num/e.jpg",c=='.'?(uint16_t)(x-5):x,y);
  }else if(c=='-' || i==0)graphics_fill(v->canvas,(uint16_t)(x+13),(uint16_t)(y+20),(uint16_t)(x+30),(uint16_t)(y+25),c=='-'?0xfd40:0);
  else graphics_fill(v->canvas,x,y,(uint16_t)(x+38),(uint16_t)(y+51),0);
  if(c!='.' && i && text[i-1]!='.')graphics_fill(v->canvas,(uint16_t)(x-5),y,x,(uint16_t)(y+51),0);
  x=(uint16_t)(x+(c=='.'?8:43));
 }
}
