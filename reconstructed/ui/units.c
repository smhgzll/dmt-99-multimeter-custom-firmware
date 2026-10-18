/* Original 17f92,18354,18974, after their common a7a4 reading dispatcher.
 * Cache values deliberately differ from unit codes (including OL's V cache).
 * A temperature preference change alone does not invalidate this cache. */
#include "units.h"
static const char *unit_image(uint8_t unit,bool f,uint8_t *key)
{
 switch(unit){
 case 6:*key=7;return "units/v.jpg";
 case 7:*key=8;return "units/mv.jpg";
 case 8:*key=11;return "units/ua.jpg";
 case 9:*key=10;return "units/ma.jpg";
 case 10:*key=9;return "units/a.jpg";
 case 11:*key=14;return "units/r.jpg";
 case 12:*key=13;return "units/kr.jpg";
 case 13:*key=12;return "units/mr.jpg";
 case 15:*key=17;return "units/nf.jpg";
 case 16:*key=16;return "units/uf.jpg";
 case 17:*key=18;return "units/mf.jpg";
 case 18:*key=21;return "units/hz.jpg";
 case 19:*key=20;return "units/khz.jpg";
 case 20:*key=19;return "units/mhz.jpg";
 case 21:*key=22;return f?"units/f.jpg":"units/c.jpg";
 default:return 0;
 }
}
static bool supported(uint8_t mode,uint8_t unit,bool recording)
{
 switch(mode){
 case 10:return unit>=15 && unit<=17;
 case 11:case 12:case 14:case 20:return unit==6 || unit==7 || (unit>=11 && unit<=13);
 case 13:return recording && (unit==6 || unit==7 || (unit>=11 && unit<=13));
 case 15:return unit>=18 && unit<=20;
 case 16:case 17:case 21:case 22:return unit>=8 && unit<=10;
 case 18:return unit==21;
 default:return false;
 }
}
void units_regular(const dmt_view *v,uint8_t mode,uint8_t unit,uint8_t layout,bool f,uint8_t *cache)
{
 uint16_t x=layout?248:251;
 if(!supported(mode,unit,false)){
  if(mode==11 || mode==12 || mode==14 || mode==20){view_fill(v,x,128,x+35,163,0);*cache=25;}
  return;
 }
 uint8_t key;const char *path=unit_image(unit,f,&key);
 if(*cache==key)return;
 *cache=key;view_fill(v,x,128,x+35,163,0);view_image(v,path,x,128);
}
void units_recording(const dmt_view *v,uint8_t mode,uint8_t unit,uint8_t layout,bool f,uint8_t *cache)
{
 if(!supported(mode,unit,true) || mode==20)return;
 uint8_t key;const char *path=unit_image(unit,f,&key);if(mode==14 && unit==6)key=6;
 if(*cache==key)return;
 *cache=key;uint16_t x=layout?170:130;view_fill(v,x,30,x+35,49,0);view_image(v,path,x,30);
 const char *text=0;
 if(unit==6 && (mode==11 || mode==12 || mode==13)){
  view_fill(v,209,219,238,234,0);view_fill(v,107,219,136,234,0);text="V  ";
 }else if(unit==10)text="A  ";else if(unit==21)text=f?"J ":"Z ";
 if(text){view_text(v,209,219,20,60,16,0xffff,text);view_text(v,107,219,20,60,16,0xffff,text);}
}
void units_monitor(const dmt_view *v,uint8_t mode,uint8_t unit,uint8_t layout,bool f,uint8_t *cache)
{
 uint8_t key;const char *path;
 if(mode==12){key=7;path="units/v.jpg";}
 else if((mode==16 || mode==21) && (unit==9 || unit==10))path=unit_image(unit,f,&key);
 else if(mode==18 && unit==21)path=unit_image(unit,f,&key);
 else return;
 if(*cache==key)return;
 *cache=key;uint16_t y=layout?158:168;view_fill(v,258,y,288,y+19,0);view_image(v,path,258,y);
}
