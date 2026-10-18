#include "screen.h"
#include "menu_render.h"
#include "gauge.h"
#include "menu_scene.h"
#include "thresholds.h"
#include "settings.h"
#include "format.h"
#include "units.h"
#include "settings_scene.h"
#include "history_view.h"
static void bitmap(dmt_screen *s,uint32_t a,uint16_t x,uint16_t y,uint16_t fg,uint16_t bg)
{view_bitmap(&s->view,a,x,y,fg,bg);}
void screen_power_on(dmt_screen *s){reading_power_on(&s->reading);s->unit_cache=0x19;s->auto_cache=0xff;}
void screen_base(dmt_screen *s,dmt_control *c,const uint8_t frame[13],uint16_t settings,uint8_t battery)
{
 const dmt_view *v=&s->view;
 if(c->view==1){
  if(!s->seen_view)s->seen_view=1;
  if(!c->layout){view_image(v,"motif2/mode1_adorn1.jpg",48,56);view_image(v,"motif2/mode1_adorn2.jpg",48,163);bitmap(s,0x26398,205,202,0,0x4228);bitmap(s,0x26338,81,202,0,0x4228);}
  else{view_image(v,"cell_rin.jpg",0,213);view_image(v,"gImage.jpg",30,0);}
 }else if(c->view==2){
  view_border(v,36,52,240,235,0x4228);view_border(v,242,52,318,235,0x4228);view_image(v,(settings&7)?"mode/mode2_max.jpg":"mode/mode2_max_c.jpg",36,218);
  for(unsigned y=68;y<236;y+=21)for(unsigned x=260;x<=274;x+=7)view_line(v,x,y,x+4,y,0xffff);
 }else if(!c->layout)view_image(v,"motif2/mode3_value.jpg",0,207);
 else{char p[96];view_image(v,"cell_rin.jpg",0,213);dmt_format(p,sizeof p,"%smode3_value.jpg",settings_language_prefix(settings));view_image(v,p,9,191);}
 uint32_t args[8]={0};view_trace(v,0x9caa,args,0);dmt_view quiet=*v;quiet.trace=0;reading_render(&s->reading,c,frame,settings,&quiet);
 args[0]=battery;
 if(!c->layout){view_image(v,"motif2/electric_frame.jpg",1,0);view_trace(v,0x14e48,args,0);if(v->status)status_battery(v->status,battery,0);}
 else{view_trace(v,0x14d62,args,0);if(v->status)status_battery(v->status,battery,1);view_image(v,"cell/-cell.jpg",10,2);view_image(v,"cell/--cell.jpg",2,2);view_image(v,"cell/cell-.jpg",261,2);view_image(v,"cell/cell--.jpg",309,2);}
}
void screen_initial(dmt_screen *s,dmt_control *c,const uint8_t frame[13],uint16_t settings,uint8_t battery)
{
 if(!s->splash_seen){s->splash_seen=true;if(!s->logo_failed || s->logo_failed(s->context))view_image(&s->view,"0:/on.jpg",0,0);if(s->backlight)s->backlight(s->context,settings_backlight_compare(settings));if(s->delay_ms)s->delay_ms(s->context,1000);}
 view_fill(&s->view,0,0,319,239,0);screen_base(s,c,frame,settings,battery);s->normal=true;
}
void screen_event(dmt_screen *s,const dmt_control *c,uint16_t settings,enum dmt_effect effect,bool hold_toggle)
{
 const dmt_view *v=&s->view;
 if(effect==DMT_EFFECT_RELATIVE_ON || effect==DMT_EFFECT_RELATIVE_OFF){
  if(effect==DMT_EFFECT_RELATIVE_ON)view_image(v,"mode/rel.jpg",205,c->layout?64:202);
  else if(c->layout)view_fill(v,205,64,241,79,0);else bitmap(s,0x26398,205,202,0,0x4228);
  return;
 }
 if(effect==DMT_EFFECT_AUTO_READING){
  uint8_t key=c->ac_dc_code==2?5:c->ac_dc_code==1?4:c->unit>=11 && c->unit<=13?14:255;
  if(s->auto_cache==key)return;
  s->auto_cache=key;bool en=(settings&7)!=0;unsigned y=c->layout?150:34;
  if(key==255)view_fill(v,74,y-3,241,y+24,0);
  else if(key==14 || c->mode==20){char path[48];dmt_format(path,sizeof path,"mode/%s%s.jpg",key==14?"r":key==5?"dc_v":"ac_v",en?"":"_c");view_image(v,path,en?119:107,y);}
  return;
 }
 if(effect==DMT_EFFECT_HOLD_ON || effect==DMT_EFFECT_HOLD_OFF){
  if(c->view==1){
   if(effect==DMT_EFFECT_HOLD_ON)view_image(v,"mode/hold.jpg",c->layout?67:81,c->layout?64:202);
   else if(!c->layout)bitmap(s,0x26338,81,202,0,0x4228);
   else{if(!hold_toggle && s->debug)s->debug(s->context,"LCD_Fill");view_fill(v,hold_toggle?62:67,64,hold_toggle?120:115,79,0);}
  }else if(c->view==2){
   if(effect==DMT_EFFECT_HOLD_ON)view_image(v,"mode/hold.jpg",255,25);else view_fill(v,255,25,303,40,0);
  }else{
   unsigned y=c->layout?174:184;
   if(effect==DMT_EFFECT_HOLD_ON)view_image(v,"mode/hold.jpg",136,y);else view_fill(v,136,y,184,y+15,0);
  }
  return;
 }
 if(effect!=DMT_EFFECT_MODE_CHANGED)return;
 static const char *names[]={"cap","r","dc_v","ac_v","ol","hz","dc_a","dc_a","temp",0,0,"ac_a","ac_a"};
 if(c->mode<10 || c->mode>22 || !names[c->mode-10]){
  if(c->view==1 && c->mode==20)view_fill(v,100,c->layout?150:34,225,c->layout?175:59,0);
  return;
 }
 bool english=(settings&7)!=0;const char *name=names[c->mode-10];char path[64];
 unsigned x,y;bool current=c->mode==16 || c->mode==17 || c->mode==21 || c->mode==22;
 if(c->view==2){
  if(!english)view_fill(v,5,178,35,191,0);
  x=english?0:8;y=english?113:68;
  if(english){
   if(c->mode==12)name="ac_v";
   if(c->mode==13)name="dc_v";
   if(c->mode==17)name="dc_ma";
   if(c->mode==22)name="ac_a-";
   if(c->mode==18)name="tem";
  }else if(c->mode==18)name="tem";
 }else if(c->view==1){
  y=c->layout?150:34;view_fill(v,100,y,225,y+25,0);x=english?119:107;
  if(!english && current)x=93;
  if(english && c->mode==17)name="dc_a-";
  if(english && c->mode==22)name="ac_a-";
  if(english && (c->mode==21 || c->mode==22))x=126;
 }else{x=english?(c->mode==18?125:119):106;y=90;}
 dmt_format(path,sizeof path,"mode/%s%s%s.jpg",c->view==2?"mode2_":"",name,english?"":"_c");view_image(v,path,x,y);
 if(!english && current && c->view!=3)view_image(v,(c->mode==17 || c->mode==22)?"units/ma.jpg":"units/a.jpg",c->view==2?(c->mode==17 || c->mode==22?5:12):197,c->view==2?178:y+6);
}
void screen_reading(dmt_screen *s,dmt_control *c,const uint8_t frame[13],uint16_t settings)
{
 /* a7a4 selects live/NCV before the common numeric renderer. */
 if(c->mode==9){
  uint8_t level=0;for(unsigned i=0;i<4;i++)level+=measurement_segment_decode((frame[2+i]&0xf0)|(frame[3+i]&15))==16;
  if(s->view.status)status_ncv(s->view.status,level,c->layout);
 }else if(c->mode==19){
  bool active=measurement_segment_decode((frame[2]&0xf0)|(frame[3]&15))!=14;
  if(s->view.status)status_live(s->view.status,active,&s->unit_cache);
 }else reading_render(&s->reading,c,frame,settings,&s->view);
 if(c->view==1)units_regular(&s->view,c->mode,c->unit,c->layout,(settings&32)!=0,&s->unit_cache);
 else if(c->view==2)units_recording(&s->view,c->mode,c->unit,c->layout,(settings&32)!=0,&s->unit_cache);
 else units_monitor(&s->view,c->mode,c->unit,c->layout,(settings&32)!=0,&s->unit_cache);
}
/* Compatibility entry: draw only the selected state's original incremental
 * operations. menu_step_rendered owns cache checks and transition side effects. */
void screen_menu(dmt_screen *s,const dmt_menu *m,const dmt_control *c,uint16_t settings,const uint16_t words[6],const dmt_history *history)
{
 dmt_menu_paint paint={.view=&s->view,.thresholds=words,.history=history};
 menu_paint_state(&paint,m,c,settings);
}
