#include "reading.h"
#include "gauge.h"
#include "format.h"
#include <string.h>
static float absolute(float v){return v<0?-v:v;}
void reading_reset_extrema(dmt_reading *r){r->minimum=9999.0f;r->maximum=-9999.0f;}
void reading_power_on(dmt_reading *r)
{reading_reset_extrema(r);r->maximum_unit=-9999;r->minimum_unit=9999;r->previous_mode=0xff;}
void extrema_format(char *out,size_t capacity,float value)
{float a=absolute(value);const char *fmt=a<10?"%0.3f":a<100?"%0.2f":a<1000?"%0.1f":"%0.0f";dmt_format(out,capacity,fmt,(double)value);}
void reading_decode_text(char out[16],const uint8_t b[13],uint8_t mode)
{
 char digit[4];for(unsigned i=0;i<4;i++){
  uint8_t n=measurement_segment_decode((b[i+2]&0xf0)|(b[i+3]&15));
  if((int8_t)n<10)digit[i]=(char)((int8_t)n+'0');
  else if(n<=16)digit[i]="AUTOLE-"[n-10];
  else if(n==17)digit[i]=(mode==0x12 || mode==0x11 || mode==0x16)?'0':' ';
  else digit[i]='X';
 }
 unsigned decimal=(b[3]&16)?1:(b[4]&16)?2:(b[5]&16)?3:0,n=0;out[n++]=(b[2]&16)?'-':' ';
 for(unsigned i=0;i<4;i++){out[n++]=digit[i];if(i+1==decimal)out[n++]='.';}
 if(!decimal)out[n++]=' ';
 out[n]=0;
}
static void unit_text(char *s,uint8_t mode,uint8_t unit)
{
 if(mode==0xb){s[0]=unit==11?'X':unit==12?'k':'M';s[1]=unit==11?' ':'X';s[2]=0;}
 else if(mode==0xa){s[0]=unit==16?'Y':unit==15?'n':'m';s[1]='F';s[2]=0;}
 else if(mode==0x11 || mode==0x16){s[0]=unit==8?'Y':'m';s[1]='A';s[2]=0;}
 else {s[0]=unit==18?'H':unit==19?'K':'M';s[1]=unit==18?'z':'H';s[2]=unit==18?' ':'z';s[3]=0;}
}
static void extreme(const dmt_view *v,char *text,float shown,bool maximum)
{extrema_format(text,16,shown);uint16_t x=maximum?165:63;view_fill(v,x,219,(uint16_t)(x+44),233,0);view_text(v,x,219,80,40,16,0xffff,text);}
void reading_render(dmt_reading *r,dmt_control *s,const uint8_t frame[13],uint16_t settings,const dmt_view *v)
{
 reading_decode_text(s->text,frame,s->mode);float shown=s->value;
 if((s->view==1?s->numeric!=0:s->numeric==1)){
  if(s->view==1 && s->relative==1)shown=shown-s->relative_value;
  if(s->mode==0x12 && ((settings>>5)&1))shown=(float)((double)shown*1.8+32.0);
  measurement_format_value(s->text,sizeof s->text,shown,s->mode,s->unit);
 }
 if(s->view==1 || s->view==3){
  view_numeric(v,s->text,s->view,s->mode,s->unit,s->layout);
  if(s->view==3){uint32_t a[8]={0};view_trace(v,0x13264,a,0);if(v && v->monitor_alarm)v->monitor_alarm(v->context,s->mode,s->value,(settings&32)!=0,s->layout);return;}
  float gauge=absolute(shown);
  if(gauge>1000)gauge=gauge/100.f;else if(gauge>100)gauge=gauge/10.f;else if(gauge<10)gauge=gauge*10.f;
  if(!s->layout){for(unsigned i=0;i<2;i++){size_t n;uint32_t a=i?0x269a4:0x263ec;const uint8_t *p=scene_bitmap(a,&n);if(p)gauge_bitmap(v,p,n,a,i?265:0,23,0x1c9f,0x4228,gauge);}}
  else {uint32_t u=gauge<0?0:!(gauge<4294967296.0)?0xffffffffu:(uint32_t)gauge;gauge_legacy(v,(uint8_t)u);}
  return;
 }
 if(s->view!=2)return;
 view_text(v,s->layout?70:30,15,200,100,32,0xfd40,s->text);
 if(s->mode==0x12 || ((s->mode==0x11 || s->mode==0x16) && s->unit==8))view_fill(v,s->layout?150:110,15,s->layout?158:118,47,0);
 bool units=s->mode==0xb || s->mode==0xa || s->mode==0x11 || s->mode==0x16 || s->mode==0xf;
 uint32_t seed=s->numeric;
 if(units){
  bool labels=s->mode!=0xb || (shown!=0 && r->resistance_guard!='L');
  if(labels && (int32_t)s->unit<r->minimum_unit){r->minimum_unit=s->unit;unit_text(r->scratch_unit,s->mode,s->unit);view_text(v,107,219,20,60,16,0xffff,r->scratch_unit);}
  if(labels && r->maximum_unit<(int32_t)s->unit){r->maximum_unit=s->unit;
   if(s->mode==0xb && s->unit!=11 && s->unit!=12)unit_text(s->text,s->mode,s->unit);
   else {unit_text(r->scratch_unit,s->mode,s->unit);if(s->mode==0x11 || s->mode==0x16)r->scratch_unit[0]=s->unit==8?'u':'m';}
   view_text(v,209,219,20,60,16,0xffff,r->scratch_unit);
  }
  if(r->minimum>shown && s->unit==r->minimum_unit){r->minimum=shown;extreme(v,s->text,s->value,false);}
  if(r->maximum<shown && s->unit==r->maximum_unit){r->maximum=shown;extreme(v,s->text,s->value,true);}
  if(s->mode!=0xb)seed=0x200001b0;
 }else if(s->mode!=0xe){
  if(r->minimum>shown){r->minimum=shown;extreme(v,s->text,shown,false);}
  if(r->maximum<shown){r->maximum=shown;extreme(v,s->text,shown,true);}
 }
 const dmt_canvas *canvas=v && v->status?v->status->canvas:0;
 if(r->previous_mode!=s->mode){
  r->previous_mode=s->mode;r->minimum=9999;r->maximum=-9999;r->maximum_unit=-9999;r->minimum_unit=9999;
  uint32_t a[8]={0,1,seed};view_trace(v,0x17cce,a,0);graph_update(&r->graph,s->value,s->mode,s->unit,1,seed,canvas,0);
  view_fill(v,63,219,132,234,0);view_fill(v,165,219,234,234,0);
  if(s->mode==0xd){view_text(v,209,219,20,60,16,0xffff,"V  ");view_text(v,107,219,20,60,16,0xffff,"V  ");}
 }else if(!s->hold && s->text[4]!='L' && s->mode!=0xe){
  uint32_t a[8]={0,0,seed};view_trace(v,0x17cce,a,0);graph_update(&r->graph,s->value,s->mode,s->unit,0,seed,canvas,0);
 }
}
