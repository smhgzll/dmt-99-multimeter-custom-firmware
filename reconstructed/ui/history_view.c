#include "history_view.h"
#include "monitor.h"
#include "reading.h"
#include "format.h"
#include <string.h>
void history_unit_update(const dmt_view *v,uint8_t unit,uint16_t y,uint8_t *previous_unit,uint16_t *previous_y,bool f)
{
 compact_unit_draw(v,unit,y,0xfd40,f);
 if(*previous_y)compact_unit_draw(v,*previous_unit,*previous_y,0xffff,f);
 *previous_unit=unit;*previous_y=y;
}
void history_strip_render(const dmt_view *v,const char *text,const char *previous,uint8_t row,bool wrapped,uint8_t unit,uint8_t *previous_unit,uint16_t *previous_y,bool f)
{
 if(row<1 || row>8)return;
 uint16_t y=(uint16_t)(row*22+35);view_fill(v,243,y,299,y+18,0);view_text(v,243,y,80,40,16,0xfd40,text);
 history_unit_update(v,unit,(uint16_t)(row*22+33),previous_unit,previous_y,f);
 if(row!=1)view_text(v,243,y-22,80,40,16,0xffff,previous);
 if(wrapped)view_text(v,243,211,80,40,16,0xffff,previous);
}
static void history_unit(char out[4],uint8_t unit,bool f)
{
 compact_unit_text(out,unit,f);
 if(unit==20){out[0]='M';out[1]='H';out[2]='z';out[3]=0;}
 if(unit==17){out[0]='m';out[1]='F';out[2]=0;}
 if(out[1]==' ' && out[2]==' ')out[1]=0;
 else if(out[2]==' ')out[2]=0;
}
void history_page_render(const dmt_view *v,const dmt_history *history,uint8_t page,uint8_t rows,uint8_t layout,bool english,bool f,bool scrollbar)
{
 uint8_t index=(uint8_t)(page*5-4);uint16_t y=53;
 for(unsigned row=0;row<rows;row++,index++,y=(uint16_t)(y+(layout?42:35))){
  if(index==0 || index>history->count || index>30)return;
  dmt_history_record record=history->records[index-1];uint8_t mode=record.metadata&63,unit=(record.metadata>>6)&63;
  float value=history_decode(record);char text[32];dmt_format(text,sizeof text,index<10?"0%u.":"%u.",(unsigned)index);view_text(v,132,y,30,20,16,0xffff,text);
  if(scrollbar && v && v->status)status_scrollbar(v->status,page,layout);
  const char *base=0;unsigned x=169;
  switch(mode){case 10:base="cap";break;case 11:base="res";break;case 12:base="_v";x=162;break;case 13:base="~v";x=162;break;case 14:base="ol";x=155;break;case 15:base="hz";break;case 16:case 17:base="_a";x=162;break;case 18:base="temp";break;case 21:case 22:base="~a";x=162;break;default:break;}
  if(base){char path[48];dmt_format(path,sizeof path,"history/%s%s.jpg",base,english?"":"_c");if(english)x=mode==18?160:155;view_image(v,path,(uint16_t)x,(uint16_t)(y+!english));}
  if(mode==18 && f)value=(float)((double)value*1.8+32);
  uint32_t bits;memcpy(&bits,&value,4);
  if(bits==0x476eee00)memcpy(text,"0.L",4);else if(bits==0x477fff00)memcpy(text,".0L",4);else extrema_format(text,sizeof text,value);
  view_text(v,242,y,80,20,16,0xffff,text);history_unit(text,unit,f);if(text[0])view_text(v,290,y,20,60,16,0xffff,text);
 }
}
