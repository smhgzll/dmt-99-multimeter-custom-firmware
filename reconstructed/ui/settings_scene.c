/* Settings scenes 153d0,15e9e,16242; brightness142a4/1432e and reset152a8.
 * Coordinates, ordering, colors and stock setting side effects are retained.
 * Scene rendering does not commit a settings image to hardware. */
#include "settings_scene.h"
#include "settings.h"
#include "gauge.h"
#include "format.h"
static void chinese(const dmt_view *v,unsigned x,unsigned y,uint32_t a,unsigned size,uint16_t c)
{view_chinese(v,x,y,scene_data(a,1),size,c);}
static void uncheck(const dmt_view *v,unsigned x,unsigned y,unsigned width,unsigned height)
{view_border(v,x,y,x+width,y+height,0xffff);view_fill(v,x+1,y+1,x+width-1,y+height-1,0);}
void settings_select_render(const dmt_view *v,bool classic,int page,int option,uint16_t *word)
{
 bool en=(*word&7)!=0;unsigned old_theme=(*word>>10)&3,old_timeout=(*word>>6)&3;
 uint16_t color=classic?0xfd40:0x1c9f;unsigned size=classic?24:16;
 if(page==2){
  bool pick_c=option==2;unsigned x=classic?153:160,old_y=classic?(pick_c?205:161):(pick_c?177:140);
  uncheck(v,x,old_y,classic?19:20,classic?19:20);
  chinese(v,classic?180:207,classic?(pick_c?202:158):(pick_c?176:138),pick_c?0x25234:0x25230,24,0xffff);
  if(!pick_c && option!=3)return;
  settings_access(word,SETTINGS_TEMPERATURE,1,pick_c?0:1);
  unsigned y=classic?(pick_c?161:205):(pick_c?140:177);view_fill(v,x,y,x+(classic?19:20),y+(classic?19:20),color);
  chinese(v,classic?180:207,classic?(pick_c?158:202):(pick_c?138:176),pick_c?0x25230:0x25234,24,color);return;
 }
 if(page==3){
  static const char *const labels[]={"OFF","5min","15min","20min"};
  unsigned x=classic?153:160,y=classic?41+old_timeout*44:74+old_timeout*31;
  uncheck(v,x,y,classic?19:20,classic?19:20);view_ascii(v,classic?180:201,classic?38+old_timeout*44:76+old_timeout*31,labels[old_timeout],0xffff,0xffff,size);
  unsigned value=option==1?0:option==2?1:option==3?2:3;settings_auto_off_set(word,value);
  y=classic?41+value*44:74+value*31;
  if(!classic)view_fill(v,x,y,x+20,y+20,color);
  view_ascii(v,classic?180:201,classic?38+value*44:76+value*31,labels[value],color,color,size);
  if(classic)view_fill(v,x,y,x+19,y+19,color);
  return;
 }
 if(page!=1)return;
 if((((unsigned)option-1u) & 255u)<2){
  unsigned old=old_theme!=0,x=classic?153:(old?252:165),y=classic?41+44*old:52;
  uncheck(v,x,y,classic?19:21,classic?19:9);
  if(classic){if(en)view_ascii(v,180,old?82:38,old?"Theme2":"Theme1",0xffff,0xffff,24);else chinese(v,180,old?82:38,old?0x25088:0x25078,24,0xffff);}
  else{if(en)view_ascii(v,old?238:161,69,old?"Theme2":"Theme1",0xffff,0xffff,16);else{chinese(v,old?246:159,71,0x25078,16,0xffff);view_ascii(v,old?278:191,71,old?"2":"1",0xffff,0xffff,16);}}
  unsigned next=option==1?0:1;settings_access(word,SETTINGS_LAYOUT,1,next);x=classic?153:(next?252:165);y=classic?41+44*next:52;view_fill(v,x,y,x+(classic?19:21),y+(classic?19:9),color);
  if(classic){if(en)view_ascii(v,180,next?82:38,next?"Theme2":"Theme1",color,color,24);else{chinese(v,180,next?82:38,0x25078,24,color);view_ascii(v,228,next?82:38,"1",color,color,24);}}
  else{if(en)view_ascii(v,next?238:161,69,next?"Theme2":"Theme1",color,color,16);else{chinese(v,next?246:159,71,0x25078,16,color);view_ascii(v,next?278:191,71,next?"2":"1",color,color,16);}}
  return;
 }
 unsigned x=classic?153:160,y=classic?(en?205:161):(en?179:139);
 uncheck(v,x,y,classic?19:20,classic?19:20);
 if(en)view_ascii(v,classic?180:198,classic?202:181,"English",0xffff,0xffff,size);else chinese(v,classic?180:198,classic?158:141,0x25098,size,0xffff);
 if(option!=3 && option!=4)return;
 settings_language_set(word,option==4);y=classic?(option==4?205:161):(option==4?179:139);view_fill(v,x,y,x+(classic?19:20),y+(classic?19:20),color);
 if(option==4)view_ascii(v,classic?180:198,classic?202:181,"English",color,color,size);else chinese(v,classic?180:198,classic?158:141,0x25098,size,color);
}
void settings_brightness_image(const dmt_view *v,int selected,int page,unsigned level,uint16_t *settings)
{
 unsigned value=level==1?1:level==2?2:0;char path[48];
 dmt_format(path,sizeof path,"menu/lighting%u%s.jpg",value+1,selected?"_":"");view_image(v,path,140,page==2?152:55);
 if(selected)settings_access(settings,SETTINGS_FIELD_BITS_3_4,1,value);
}
void settings_brightness_bar(const dmt_view *v,unsigned level,uint16_t *settings)
{
 unsigned value=level<3?level:0;view_cut_fill(v,154,54,284,70,2,0);
 if(value==0)view_cut_fill(v,154,54,284,70,2,0x1c9f);
 else view_cut_fill(v,153,53,value==1?241:197,71,2,0x1c9f);
 settings_access(settings,SETTINGS_FIELD_BITS_3_4,1,value);view_cut_border(v,153,53,285,71,2,0x4228);
}
void settings_reset_choices(const dmt_view *v,int selected,bool en)
{
 uint16_t a=selected!=0 && selected!=1?0:0xffff,b=selected==1?0:0xffff;
 uint16_t ca=selected!=0 && selected!=1?0x1c9f:0x528a,cb=selected==1?0x1c9f:0x528a;
 if(selected!=1)view_cut_border(v,174,157,267,182,3,0x4228);
 if(selected!=2)view_cut_border(v,174,191,267,216,3,0x4228);
 view_cut_fill(v,174,157,267,182,3,ca);view_cut_fill(v,174,191,267,216,3,cb);
 if(en){view_ascii(v,201,161,"Reset",a,a,16);view_ascii(v,197,195,"Return",b,b,16);}
 else{chinese(v,184,164,0x24f48,12,a);chinese(v,208,198,0x24f58,12,b);}
}
void settings_scene_theme(const dmt_view *v,uint8_t layout,uint16_t settings)
{
 bool theme=((settings>>10)&3)!=0,en=(settings&7)!=0;
 uint16_t one=theme?0xffff:(layout?0xfd40:0x1c9f),two=theme?(layout?0xfd40:0x1c9f):0xffff;
 uint16_t cn=en?0xffff:(layout?0xfd40:0x1c9f),english=en?(layout?0xfd40:0x1c9f):0xffff;
 if(!layout){
  view_cut_border(v,144,41,294,96,8,0x4228);view_cut_border(v,144,124,294,212,8,0x4228);
  if(!theme){view_fill(v,165,52,186,61,0x1c9f);view_fill(v,253,53,272,60,0);view_border(v,252,52,273,61,0xffff);}
  else{view_fill(v,252,52,273,61,0x1c9f);view_fill(v,166,53,185,60,0);view_border(v,165,52,186,61,0xffff);}
  if(!en){view_fill(v,160,139,180,159,0x1c9f);view_fill(v,161,180,179,198,0);view_border(v,160,179,180,199,0xffff);}
  else{view_fill(v,160,179,180,199,0x1c9f);view_fill(v,161,140,179,158,0);view_border(v,160,139,180,159,0xffff);}
  chinese(v,198,141,0x25098,16,cn);view_ascii(v,198,181,"English",english,english,16);
  if(!en){chinese(v,201,17,0x25078,16,0xffff);chinese(v,201,102,0x250cc,16,0xffff);chinese(v,159,71,0x25078,16,one);view_ascii(v,191,71,"1",one,one,16);chinese(v,246,71,0x25078,16,two);view_ascii(v,278,71,"2",two,two,16);}
  else{view_ascii(v,201,17,"Theme",0xffff,0xffff,16);view_ascii(v,185,102,"Language",0xffff,0xffff,16);view_ascii(v,161,69,"Theme1",one,one,16);view_ascii(v,238,69,"Theme2",two,two,16);}
 }else{
  if(!theme){view_fill(v,153,41,172,60,0xfd40);view_border(v,153,85,172,104,0xffff);view_fill(v,154,86,171,103,0);}
  else{view_border(v,153,41,172,60,0xffff);view_fill(v,154,42,171,59,0);view_fill(v,153,85,172,104,0xfd40);}
  if(!en){view_fill(v,153,161,172,180,0xfd40);view_border(v,153,205,172,224,0xffff);view_fill(v,154,206,171,223,0);}
  else{view_fill(v,153,205,172,224,0xfd40);view_border(v,153,161,172,180,0xffff);view_fill(v,154,162,171,179,0);}
  view_line(v,135,113,311,113,0x4228);chinese(v,180,158,0x25098,24,cn);view_ascii(v,180,202,"English",english,english,24);
  if(!en){chinese(v,139,9,0x25078,24,0xffff);chinese(v,139,125,0x250cc,24,0xffff);view_ascii_char(v,184,9,':',24,0xffff);view_ascii_char(v,184,125,':',24,0xffff);chinese(v,180,38,0x25078,24,one);view_ascii_char(v,228,38,'1',24,one);chinese(v,180,82,0x25078,24,two);view_ascii_char(v,228,82,'2',24,two);}
  else{view_ascii(v,139,9,"Theme:",0xffff,0xffff,24);view_ascii(v,139,125,"Language:",0xffff,0xffff,24);view_ascii(v,180,38,"Theme1",one,one,24);view_ascii(v,180,82,"Theme2",two,two,24);}
 }
}
void settings_scene_measurement(const dmt_view *v,uint8_t layout,uint16_t *settings)
{
 unsigned brightness=(*settings>>3)&3;bool f=(*settings&32)!=0,en=(*settings&7)!=0;uint16_t selected=layout?0xfd40:0x1c9f;
 if(!layout){
  view_fill(v,163,11,300,141,0);view_fill(v,142,22,311,219,0);
  /*14408 scrollbar contract is emitted; draw through the existing canvas adapter. */
  uint32_t args[8]={3};view_trace(v,0x14408,args,0);if(v && v->status)status_scrollbar(v->status,3,layout);
  view_cut_border(v,144,124,294,212,8,0x4228);view_bitmap(v,0x25488,159,25,0,0xffff);
  view_bitmap(v,0x26f5c,en?147:160,99,0,0xffff);
  if(en){view_ascii(v,193,27,"Brightness",0xffff,0xffff,16);view_ascii(v,165,100,"Temperature unit",0xffff,0xffff,16);}
  else{chinese(v,193,27,0x247c0,16,0xffff);chinese(v,181,100,0x247c8,16,0xffff);}
  if(!f){view_fill(v,160,140,180,160,selected);view_border(v,160,177,180,197,0xffff);}
  else{view_border(v,160,140,180,160,0xffff);view_fill(v,160,177,180,197,selected);}
  chinese(v,207,138,0x25230,24,f?0xffff:selected);chinese(v,207,176,0x25234,24,f?selected:0xffff);settings_brightness_bar(v,brightness,settings);
 }else{
  view_fill(v,131,0,320,240,0);uint32_t args[8]={3};view_trace(v,0x14408,args,0);if(v && v->status)status_scrollbar(v->status,3,layout);
  if(en){view_ascii(v,139,9,"Brightness:",0xffff,0xffff,24);view_ascii(v,139,125,"Temperature unit:",0xffff,0xffff,16);}
  else{chinese(v,139,9,0x247c0,24,0xffff);chinese(v,139,125,0x247c8,24,0xffff);view_ascii_char(v,184,9,':',24,0xffff);view_ascii_char(v,231,125,':',24,0xffff);}
  if(!f){view_fill(v,153,161,172,180,selected);view_border(v,153,205,172,224,0xffff);}
  else{view_fill(v,153,205,172,224,selected);view_border(v,153,161,172,180,0xffff);}
  chinese(v,180,158,0x25230,24,f?0xffff:selected);chinese(v,180,202,0x25234,24,f?selected:0xffff);view_line(v,135,113,311,113,0x4228);settings_brightness_image(v,1,3,brightness,settings);
 }
}
void settings_scene_auto_off(const dmt_view *v,uint8_t layout,uint16_t settings)
{
 bool en=(settings&7)!=0;unsigned timeout=(settings>>6)&3;uint32_t args[8]={6};view_trace(v,0x14408,args,0);if(v && v->status)status_scrollbar(v->status,6,layout);
 if(!layout){view_fill(v,150,15,300,20,0);view_fill(v,130,40,150,208,0);view_fill(v,140,20,310,218,0);view_cut_border(v,144,61,298,200,8,0x4228);if(en)view_ascii(v,160,39,"AUTO shut_down",0xffff,0xffff,16);else chinese(v,184,39,0x24f60,16,0xffff);}
 else{view_fill(v,131,0,320,240,0);if(en)view_ascii(v,139,15,"AUTO shut_down:",0xffff,0xffff,16);else{chinese(v,139,9,0x24f60,24,0xffff);view_ascii_char(v,231,9,':',24,0xffff);}}
 static const char *const labels[]={"OFF","5min","15min","20min"};
 for(unsigned i=0;i<4;i++){
  unsigned y=layout?41+44*i:74+31*i;uint16_t c=timeout==i?(layout?0xfd40:0x1c9f):0xffff;
  if(timeout==i)view_fill(v,layout?153:160,y,layout?172:180,y+(layout?19:20),c);else view_border(v,layout?153:160,y,layout?172:180,y+(layout?19:20),c);
  view_ascii(v,layout?180:201,layout?38+44*i:76+31*i,labels[i],c,c,layout?24:16);
 }
}
