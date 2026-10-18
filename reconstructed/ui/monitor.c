#include "monitor.h"
#include "thresholds.h"
#include "gauge.h"
#include "format.h"
void monitor_threshold_render(const dmt_view *v,const uint16_t words[6],uint8_t mode,uint8_t layout,bool english,bool f)
{
 if(!layout){
  view_image(v,"motif2/mode3_value.jpg",0,207);
  if(english)view_ascii(v,125,214,"Threshold",0xffff,0xffff,16);
  else view_chinese(v,136,211,scene_data(0x25208,1),24,0xffff);
 }else{view_fill(v,30,195,98,218,0);view_fill(v,240,195,297,218,0);}
 unsigned group;
 if(mode==12 || mode==13)group=0;else if(mode==16 || mode==21)group=2;else if(mode==18)group=4;else return;
 char low[32],high[32];int32_t a=threshold_decode(words[group]),b=threshold_decode(words[group+1]);
 if(group==2){dmt_format(low,sizeof low,"%0.2f",(double)((float)a/100.f));dmt_format(high,sizeof high,"%0.2f",(double)((float)b/100.f));}
 else{if(group==4 && f){a=(int32_t)((double)a*1.8+32);b=(int32_t)((double)b*1.8+32);}dmt_format(low,sizeof low,"%d",a);dmt_format(high,sizeof high,"%d",b);}
 if(!layout)view_ascii(v,25,210,low,0xffff,0xffff,24);else view_text(v,25,194,80,40,24,0xffff,low);
 if(group!=4){
  if(!layout)view_ascii(v,group==2?90:80,215,group==2?"A":"V",0xffff,0xffff,16);
  else{const char *p=group==2?"units/a.jpg":"units/v.jpg";view_image(v,p,80,201);view_image(v,p,278,201);}
 }
 if(!layout)view_ascii(v,235,210,high,0xffff,0xffff,24);else view_text(v,225,194,80,40,24,0xffff,high);
 if(!layout){
  if(group==4){const uint8_t *code=scene_data(f?0x25234:0x25230,1);view_chinese(v,80,215,code,16,0xffff);view_chinese(v,280,215,code,16,0xffff);}
  else view_ascii(v,group==2?288:280,215,group==2?"A":"V",0xffff,0xffff,16);
 }else if(group==4){const char *p=f?"units/f.jpg":"units/c.jpg";view_image(v,p,80,201);view_image(v,p,278,201);}
}
void monitor_alarm_render(const dmt_view *v,const uint16_t words[6],uint8_t mode,float value,bool fahrenheit,uint8_t layout,uint8_t *alarm)
{
 uint8_t old=*alarm;
 if(!threshold_alarm(words,mode,value,fahrenheit,alarm) || old==*alarm)return;
 if(v && v->buzzer)v->buzzer(v->context,*alarm!=0);
 const char *a=*alarm==0?"mode3_medi1.jpg":*alarm==1?"mode3_min1.jpg":"mode3_max1.jpg";
 const char *b=*alarm==0?"mode3_medi2.jpg":"mode3_max2.jpg";
 const char *c=*alarm==0?"mode3_medi3.jpg":*alarm==1?"mode3_min3.jpg":"mode3_max3.jpg";
 uint16_t y=layout?20:25;view_image(v,a,19,y);view_image(v,b,120,y);view_image(v,c,199,y);
}
void compact_unit_text(char out[4],uint8_t unit,bool f)
{
 const char *p=0;
 switch(unit){case 6:p="V  ";break;case 7:p="mV ";break;case 8:p="YA ";break;case 9:p="mA ";break;case 10:p="A  ";break;case 11:p="X  ";break;case 12:p="kX ";break;case 13:p="MX ";break;case 15:case 17:p="nF ";break;case 16:p="YF ";break;case 18:p="Hz ";break;case 19:p="kHz";break;case 20:p="mHz";break;case 21:p=f?"J  ":"Z  ";break;default:break;}
 for(unsigned i=0;i<4;i++)out[i]=p?p[i]:0;
}
void compact_unit_draw(const dmt_view *v,uint8_t unit,uint16_t y,uint16_t color,bool f)
{char out[4];compact_unit_text(out,unit,f);if(out[0])view_text(v,288,(uint16_t)(y+2),60,20,16,color,out);}
