/* Ordered erases/repaints from 1598c,15af6,164d4,169d4. This is deliberately
 * incremental: a cache hit in menu_step_rendered must not repaint a scene.
 * Original ROM strings and the odd overlapping cursor rectangles are retained. */
#include "menu_render.h"
#include "menu_scene.h"
#include "settings_scene.h"
#include "settings.h"
#include "thresholds.h"
#include "history_view.h"
#include "gauge.h"
#include "format.h"
static void image(const dmt_view *v,uint16_t word,const char *name,unsigned x,unsigned y)
{char path[96];const char *prefix=settings_language_prefix(word);if(prefix){dmt_format(path,sizeof path,"%s%s",prefix,name);view_image(v,path,x,y);}}
static void cn(const dmt_view *v,unsigned x,unsigned y,uint32_t a,unsigned size,uint16_t color)
{view_chinese(v,x,y,scene_data(a,1),size,color);}
static void first_colon(const dmt_view *v,unsigned x,unsigned y)
{uint32_t args[8]={x,y,':',16,0xffff,0x1f,1};view_trace(v,0x83d6,args,0);if(v && v->status && v->status->canvas)graphics_ascii(v->status->canvas,x,y,':',16,0xffff,0x1f,true);}
static void category(const dmt_view *v,uint8_t layout,uint16_t word,int initial,int selected,int previous)
{if(!layout)menu_category_compact(v,(word&7)!=0,initial,selected,previous);else if(!(word&7))menu_category_large(v,initial,selected,previous);}
void menu_paint_clear(const dmt_menu_paint *p)
{
 if(!p)return;
 uint32_t args[8]={0};view_trace(p->view,0x876a,args,0);
 if(p->view && p->view->status && p->view->status->canvas)graphics_fill(p->view->status->canvas,0,0,319,239,0);
}
void menu_paint_begin(const dmt_menu_paint *p,uint8_t layout,uint16_t word)
{
 if(!p)return;
 const dmt_view *v=p->view;menu_paint_clear(p);
 if(!layout){
  view_image(v,"motif2/menu_adorn1.jpg",0,0);view_image(v,"motif2/menu_adorn2.jpg",0,211);
  view_line(v,0,28,120,29,0x4228);view_line(v,120,28,160,7,0x4228);view_line(v,160,7,320,7,0x4228);
  view_line(v,0,211,120,210,0x4228);view_line(v,120,211,160,232,0x4228);view_line(v,160,232,320,232,0x4228);
 }
 category(v,layout,word,1,1,0);
}
static void compact_clear(const dmt_view *v)
{view_fill(v,150,15,300,20,0);view_fill(v,130,40,150,208,0);view_fill(v,140,20,320,218,0);}
static void history_heading(const dmt_view *v,uint8_t layout,uint16_t word,int previous,bool down)
{
 if(!layout){
  if(down)view_fill(v,150,20,320,220,0);else compact_clear(v);
  category(v,layout,word,0,3,previous);
  if(word&7){view_ascii(v,169,26,(const char *)scene_data(0x25158,1),0xfd40,0xfd40,16);view_ascii(v,238,26,"Measurements",0xfd40,0xfd40,16);}
  else{cn(v,169,26,0x25148,16,0xfd40);cn(v,245,26,0x25150,16,0xfd40);}
 }else if(!(word&7)){
  /* The reverse transition updates the category before clearing the pane. */
  if(down)view_fill(v,131,0,320,240,0);
  category(v,layout,word,0,3,previous);
  if(!down)view_fill(v,131,0,320,240,0);
  cn(v,159,12,0x25148,24,0xfd40);cn(v,238,12,0x25150,24,0xfd40);
 }
}
void menu_paint_navigate(const dmt_menu_paint *p,uint8_t state,uint8_t event,uint8_t layout,uint16_t *word)
{
 if(!p || (event!=1 && event!=2))return;
 const dmt_view *v=p->view;
 if((event==1 && state==1) || (event==2 && state==12)){category(v,layout,*word,0,2,state==1?1:3);return;}
 if((event==1 && state==5) || (event==2 && state==13)){history_heading(v,layout,*word,state==5?2:4,event==1);return;}
 if((event==1 && state==12) || (event==2 && state==29)){
  if(!layout)compact_clear(v);
  category(v,layout,*word,0,4,state==12?3:5);
  if(layout)view_fill(v,131,0,320,240,0);
  settings_scene_theme(v,layout,*word);return;
 }
 if((event==1 && state==13) || (event==2 && state==32)){
  if(!layout)compact_clear(v);
  category(v,layout,*word,0,5,state==13?4:6);return;
 }
 if((event==1 && state==29) || (event==2 && state==1)){category(v,layout,*word,0,6,state==29?5:1);return;}
 if((event==1 && state==32) || (event==2 && state==5)){category(v,layout,*word,0,1,state==32?6:2);return;}
 if(event==1 && state==15){settings_scene_measurement(v,layout,word);return;}
 if(event==1 && state==24){settings_scene_auto_off(v,layout,*word);return;}
 if(event==2 && state==25){settings_scene_measurement(v,layout,word);return;}
 if(event==2 && state==22){
  if(!layout){view_fill(v,150,15,300,20,0);view_fill(v,130,40,150,208,0);view_fill(v,140,20,320,218,0);}
  else view_fill(v,131,0,320,240,0);
  uint32_t args[8]={1};view_trace(v,0x14408,args,0);if(v && v->status)status_scrollbar(v->status,1,layout);
  settings_scene_theme(v,layout,*word);
 }
}
void menu_paint_back(const dmt_menu_paint *p,uint8_t layout,uint16_t word)
{
 if(!p)return;
 const dmt_view *v=p->view;
 if(!layout){view_fill(v,130,40,150,208,0);view_fill(v,140,20,320,218,0);view_fill(v,317,11,319,228,0);}
 else{view_fill(v,131,0,320,240,0);view_fill(v,315,5,317,234,0);}
 settings_scene_theme(v,layout,word);
}
void menu_paint_select(const dmt_menu_paint *p,uint8_t state,uint8_t layout,uint16_t word)
{
 if(!p)return;
 const dmt_view *v=p->view;bool en=(word&7)!=0;
 if(state==14 || state==15){
  if(en!=(state==15)){
   if(layout){
    if(state==14)view_fill(v,0,0,130,239,0);
    view_fill(v,139,9,209,33,0);view_fill(v,139,125,249,149,0);view_fill(v,180,38,280,62,0);view_fill(v,180,82,310,106,0);
    settings_select_render(v,true,1,state-11,&word);
    if(state==14)menu_category_large(v,1,4,0);
    else{image(v,word,"mode_set.jpg",0,0);image(v,word,"install.jpg",0,120);}
   }else{
    settings_select_render(v,false,1,state-11,&word);
    view_fill(v,0,29,120,210,0);view_fill(v,179,16,276,36,0);view_fill(v,179,102,276,121,0);view_fill(v,157,71,285,89,0);
    menu_category_compact(v,(word&7)!=0,1,4,0);
   }
  }
  settings_scene_theme(v,layout,word);return;
 }
 if((state==20 || state==21) && settings_access(&word,SETTINGS_LAYOUT,0,0)==(uint8_t)(21-state)){
  settings_access(&word,SETTINGS_LAYOUT,1,state-20);layout=state-20;menu_paint_clear(p);
  if(!layout){view_image(v,"motif2/menu_adorn1.jpg",0,0);view_image(v,"motif2/menu_adorn2.jpg",0,211);menu_category_compact(v,en,1,4,0);}
  else if(!en)menu_category_large(v,1,4,0);
  else{image(v,word,"mode_set.jpg",0,0);image(v,word,"install.jpg",0,120);}
  settings_scene_theme(v,layout,word);view_border(v,layout?141:155,layout?78:47,layout?309:208,layout?111:91,layout?0xfd40:0x1c9f);return;
 }
 if(state==23 || state==24)settings_select_render(v,layout,2,state-21,&word);
 if(state>=25 && state<=28)settings_select_render(v,layout,3,state-24,&word);
}
void menu_threshold_all(const dmt_view *v,uint8_t layout,bool en)
{for(unsigned i=1;i<=6;i++)menu_threshold_hint(v,!layout,en,0,i);}
static void threshold_hint(const dmt_view *v,uint8_t layout,bool en,int selected,unsigned row)
{if(layout && en)menu_threshold_images(v,en,selected,row);else menu_threshold_hint(v,!layout,en,selected,row);}
void menu_paint_state(const dmt_menu_paint *p,const dmt_menu *m,const dmt_control *c,uint16_t word)
{
 if(!p)return;
 const dmt_view *v=p->view;bool en=(word&7)!=0;uint8_t layout=c->layout,id=m->state;
 uint16_t color=layout?0xfd40:0x1c9f;
 switch(id){
 case 1:
  if(!layout){view_fill(v,130,40,150,208,0);view_fill(v,140,20,320,218,0);menu_choice_compact(v,en,1,c->view==1?1:c->view==2?2:3,0);}
  else{
   if(en){image(v,word,"mode_set.jpg",0,0);image(v,word,"mode.jpg",0,0);}
   view_fill(v,131,0,320,240,0);
   if(!en)menu_choice_large(v,1,c->view==1?1:c->view==2?2:3,0);
   else{image(v,word,c->view==1?"mode1_.jpg":"mode1.jpg",157,53);image(v,word,c->view==2?"mode2_.jpg":"mode2.jpg",157,103);image(v,word,c->view!=1 && c->view!=2?"mode3_.jpg":"mode3.jpg",157,153);}
  }break;
 case 2:case 3:case 4:{
  unsigned ys[]={52,102,152},ends[]={87,137,187};unsigned row=id-2;
  view_border(v,156,ys[row],layout?294:264,ends[row],color);
  /* Original order is selected, first peer, second peer. */
  for(unsigned i=0;i<3;i++)if(i!=row)view_border(v,156,ys[i],layout?294:264,ends[i],0);
  break;
 }
 case 5:
  if(!layout){
   view_fill(v,140,20,320,218,0);view_fill(v,130,40,150,208,0);menu_threshold_all(v,layout,en);
   if(en){view_ascii(v,166,22,"Voltagge:",0xffff,0xffff,16);view_ascii(v,166,87,"Current:",0xffff,0xffff,16);view_ascii(v,166,151,"Temperature:",0xffff,0xffff,16);}
   else{cn(v,166,22,0x24974,16,0xffff);cn(v,166,87,0x2497c,16,0xffff);cn(v,166,151,0x24984,16,0xffff);first_colon(v,201,23);view_ascii_char(v,201,86,':',16,0xffff);view_ascii_char(v,201,152,':',16,0xffff);}
  }else if(en){image(v,word,"mode_set.jpg",0,0);image(v,word,"value.jpg",0,40);view_image(v,"menu/value1.jpg",131,0);image(v,word,"voltage.jpg",136,7);image(v,word,"electric.jpg",136,84);image(v,word,"tep.jpg",136,162);}
  else{
   view_fill(v,131,0,320,240,0);menu_threshold_all(v,layout,en);
   cn(v,137,5,0x24974,16,0xffff);cn(v,137,82,0x2497c,16,0xffff);cn(v,137,160,0x24984,16,0xffff);
   first_colon(v,172,5);view_ascii_char(v,172,82,':',16,0xffff);view_ascii_char(v,172,160,':',16,0xffff);
  }
  {uint32_t args[8]={0};view_trace(v,0x126d8,args,0);if(p->thresholds && v && v->status && v->status->canvas)thresholds_draw(p->thresholds,v->status->canvas,layout);}break;
 case 6:case 7:case 8:case 9:case 10:case 11:{
  unsigned row=id-5;
  /* Some decompiler calls omit a1 here. The original register stream supplies
   * the adjacent rows in the English JPEG branch too. */
  if(row>1)threshold_hint(v,layout,en,0,row-1);
  if(row<6)threshold_hint(v,layout,en,0,row+1);
  threshold_hint(v,layout,en,1,row);break;
 }
 case 12:
  if(layout && en){image(v,word,"mode_set.jpg",0,0);image(v,word,"history.jpg",0,80);view_fill(v,131,0,320,240,0);image(v,word,"history_.jpg",131,15);}
  {uint8_t rows=p->history?(p->history->count<5?p->history->count:5):0;uint32_t args[8]={1,rows,1};view_trace(v,0x14578,args,0);
   if(p->history && v){dmt_view quiet=*v;quiet.trace=0;history_page_render(&quiet,p->history,1,rows,layout,en,(word&32)!=0,false);}}
  view_fill(v,layout?315:317,layout?5:11,layout?317:319,layout?234:228,0);break;
 case 13:if(layout && en){image(v,word,"mode_set.jpg",0,0);image(v,word,"install.jpg",0,120);}break;
 case 14:
  if(!layout){view_border(v,235,47,288,91,0);view_border(v,155,135,285,163,color);view_border(v,155,175,285,203,0);}
  else{view_border(v,141,154,309,187,color);view_border(v,141,78,309,111,0);view_border(v,141,198,309,231,0);}break;
 case 15:
  if(!layout){view_border(v,155,135,285,163,0);view_border(v,155,175,285,203,color);}
  else{view_border(v,141,198,309,231,color);view_border(v,141,154,309,187,0);}break;
 case 20:
  view_border(v,layout?141:155,layout?34:47,layout?309:208,layout?67:91,color);view_border(v,layout?141:235,layout?78:47,layout?309:288,layout?111:91,0);break;
 case 21:
  if(!layout){view_border(v,155,47,208,91,0);view_border(v,235,47,288,91,color);view_border(v,155,135,285,163,0);}
  else{view_border(v,141,78,309,111,color);view_border(v,141,34,309,67,0);view_border(v,141,154,309,187,0);}break;
 case 22:
  view_border(v,layout?136:144,layout?50:49,layout?309:293,layout?83:75,color);
  if(!layout)view_border(v,151,135,283,164,0);else view_border(v,141,154,309,187,0);break;
 case 23:
  if(!layout){view_border(v,144,49,293,75,0);view_border(v,151,135,283,164,color);view_border(v,151,173,283,202,0);}
  else{view_border(v,141,154,309,187,color);view_border(v,136,50,309,83,0);view_border(v,141,198,309,231,0);}break;
 case 24:
  if(!layout){view_border(v,151,135,283,164,0);view_border(v,151,173,283,202,color);}
  else{view_border(v,141,198,309,231,color);view_border(v,141,154,309,187,0);}break;
 case 25:case 26:case 27:case 28:{
  unsigned row=id-25;
  if(!layout){if(row==0){view_border(v,156,70,285,98,color);view_border(v,156,101,285,129,0);}else{view_border(v,156,70+(row-1)*31,285,98+(row-1)*31,0);view_border(v,156,70+row*31,285,98+row*31,color);if(row<3)view_border(v,156,70+(row+1)*31,285,98+(row+1)*31,0);}}
  else{view_border(v,141,34+row*44,309,67+row*44,color);if(row>0)view_border(v,141,34+(row-1)*44,309,67+(row-1)*44,0);if(row<3)view_border(v,141,34+(row+1)*44,309,67+(row+1)*44,0);}
  break;
 }
 case 29:
  if(!layout){view_cut_border(v,138,43,304,145,10,0x4228);view_glyph(v,en?172:184,54,0x249e8,24,0xfd40);view_ascii_char(v,en?180:192,59,'!',16,0);
   if(en){view_ascii(v,201,54,"Notice",0xfd40,0xfd40,24);view_ascii(v,189,87,"Reset all",0xffff,0xffff,16);view_ascii(v,182,112,"settings!!!",0xffff,0xffff,16);}
   else{cn(v,208,54,0x24a0c,24,0xfd40);cn(v,157,87,0x24a14,16,0xffff);cn(v,157,112,0x24a28,16,0xffff);}settings_reset_choices(v,0,en);
  }else{if(en){image(v,word,"mode_set.jpg",0,0);image(v,word,"recover.jpg",0,160);}image(v,word,"recover1.jpg",131,0);}break;
 case 30:case 31:
  if(!layout)settings_reset_choices(v,id-29,en);else image(v,word,id==30?"recover1_1.jpg":"recover1_2.jpg",143,147);break;
 case 32:
  if(!layout){view_fill(v,130,40,150,208,0);view_fill(v,140,20,320,218,0);}
  else{view_fill(v,131,0,320,240,0);if(en){view_image(v,"menu/mode_set.jpg",0,0);image(v,word,"mode_set.jpg",0,0);image(v,word,"message.jpg",0,200);}}
  view_image(v,"menu/about.jpg",131,50);{uint32_t args[8]={0};view_trace(v,0x143e0,args,0);if(v && v->status)status_about_version(v->status);}break;
 default:break; /* Original holes16..19 retain both framebuffer and cache. */
 }
}
