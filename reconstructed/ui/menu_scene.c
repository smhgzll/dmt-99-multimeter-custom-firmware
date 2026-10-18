#include "menu_scene.h"
#include "gauge.h"
#include "format.h"
void menu_threshold_images(const dmt_view *v,bool english,int selected,unsigned row)
{
 if(row<1 || row>6)return;
 static const uint8_t ys[]={27,54,104,132,182,210};unsigned y=ys[row-1];bool low=row==1 || row==3 || row==5;char path[48];
 dmt_format(path,sizeof path,"menu/%s%s%s.jpg",low?"min":"max",english?"":"_c",selected?"_":"");view_image(v,path,141,y);
 dmt_format(path,sizeof path,"menu/%s%s.jpg",row<3?"v":row<5?"a":"c",selected?"_":"");view_image(v,path,row<5?282:278,y+3);
 uint16_t color=selected?0xfd40:0x2105;view_line(v,193,y,301,y,color);view_line(v,193,y+21,301,y+21,color);view_image(v,selected?"menu/max_min_right_.jpg":"menu/max_min_right.jpg",301,y);
}
static const uint8_t *rom(uint32_t a){return scene_data(a,1);}
void view_glyph(const dmt_view *v,uint16_t x,uint16_t y,uint32_t a,uint8_t size,uint16_t color)
{
 uint32_t args[8]={x,y,a,color,color,size,1};view_trace(v,size==16?0x8b5c:0x8c64,args,(const char *)rom(a));
 if(v && v->status){dmt_view copy=*v;copy.trace=0;view_chinese(&copy,x,y,rom(a),size,color);}
}
static const uint32_t glyphs[6][6]={{0x253d0,0x253d4,0x253d8,0x253dc},{0x253e0,0x25464,0x253e4,0x253e8},{0x253ec,0x253f0,0x253f4,0x253f8},{0x253fc,0x25400,0x253e4,0x253e8},{0x25404,0x25408,0x2540c,0x25410,0x253e4,0x253e8},{0x25414,0x25418}};
static void category_row(const dmt_view *v,bool large,bool english,unsigned row,uint16_t color,bool initial)
{
 if(row>=6)return;
 static const uint16_t xsmall[6][6]={{6,36,67,97},{6,36,67,97},{5,35,66,96},{5,35,66,96},{5,23,41,59,78,97},{5,96}};
 static const uint16_t xlarge[6][6]={{5,37,69,101},{5,37,69,101},{5,37,69,101},{5,37,69,101},{0,21,42,63,84,105},{5,101}};
 static const uint16_t ys[]={36,66,97,127,158,188};
 if(large || !english){for(unsigned i=0;i<6 && glyphs[row][i];i++)view_glyph(v,large?xlarge[row][i]:xsmall[row][i],large?(uint16_t)(9+row*39):ys[row],glyphs[row][i],large?24:16,color);return;}
 uint16_t bg=initial?0:color;
 switch(row){case 0:view_ascii(v,6,36,"Mode selection",color,bg,16);break;case 1:view_ascii(v,24,61,"Threshold",color,bg,16);view_ascii(v,28,73,"settings",color,bg,16);break;case 2:view_ascii(v,6,97,"History record",color,bg,16);break;case 3:view_ascii(v,0,127,"System settings",color,bg,16);break;case 4:view_ascii(v,40,158,"Reset",color,bg,16);break;default:view_ascii(v,40,188,"About",color,bg,16);break;}
}
void menu_category_compact(const dmt_view *v,bool english,int initial,int selected,int previous)
{
 static const uint16_t top[]={29,61,91,122,152,183},bottom[]={59,89,120,150,181,210};
 if(initial){for(unsigned i=0;i<5;i++){uint16_t y=(uint16_t)(60.0+i*30.5);view_line(v,0,y,119,y,0x4228);}view_line(v,120,29,120,210,0x4228);for(unsigned i=0;i<6;i++)category_row(v,false,english,i,0xffff,true);}
 if(selected>=1 && selected<=6){view_fill(v,0,top[selected-1],119,bottom[selected-1],0x1c9f);category_row(v,false,english,selected-1,0,false);}
 if(previous>=1 && previous<=6){view_fill(v,0,top[previous-1],119,bottom[previous-1],0);category_row(v,false,english,previous-1,0xffff,false);}
}
void menu_category_large(const dmt_view *v,int initial,int selected,int previous)
{
 if(initial){for(unsigned i=1;i<6;i++)view_line(v,0,(uint16_t)(39*i),129,(uint16_t)(39*i),0x4228);view_line(v,130,0,130,234,0x4228);for(unsigned i=0;i<6;i++)category_row(v,true,false,i,0xffff,true);}
 view_fill(v,0,(uint16_t)((selected-1)*39+1),128,(uint16_t)(selected*39-2),0xfd40);
 if(selected>=1 && selected<=6)category_row(v,true,false,selected-1,0,false);
 if(previous){view_fill(v,0,(uint16_t)((previous-1)*39+1),128,(uint16_t)(previous*39-2),0);if(previous>=1 && previous<=6)category_row(v,true,false,previous-1,0xffff,false);}
}
static void choice_text(const dmt_view *v,bool large,bool english,unsigned index,uint16_t color,bool old)
{
 uint16_t y=(uint16_t)((large?58:60)+index*50);
 if(large || !english)view_chinese(v,191,y,rom(0x25380+index*12),large?24:16,color);
 else{if(old && index==1)y=160;const char *s=index==0?"Regular":index==1?"Recording":"Monitor";view_ascii(v,191,y,s,color,color,16);}
}
static void choices(const dmt_view *v,bool large,bool english,int initial,int selected,int previous)
{
 unsigned x1=large?184:182,h=large?19:17;
 if(initial==1){for(unsigned i=0;i<3;i++)choice_text(v,large,english,i,0xffff,false);for(unsigned i=0;i<3;i++)view_border(v,165,(uint16_t)(60+i*50),x1,(uint16_t)(60+i*50+h),0xffff);}
 if(selected>=1 && selected<=3){choice_text(v,large,english,selected-1,large?0xfd40:0x1c9f,false);view_fill(v,165,(uint16_t)(60+(selected-1)*50),x1,(uint16_t)(60+(selected-1)*50+h),large?0xfd40:0x1c9f);}
 if(previous>=1 && previous<=3){choice_text(v,large,english,previous-1,0xffff,true);view_fill(v,165,(uint16_t)(60+(previous-1)*50),x1,(uint16_t)(60+(previous-1)*50+h),0);view_border(v,165,(uint16_t)(60+(previous-1)*50),x1,(uint16_t)(60+(previous-1)*50+h),0xffff);}
}
void menu_choice_compact(const dmt_view *v,bool english,int initial,int selected,int previous){choices(v,false,english,initial,selected,previous);}
void menu_choice_large(const dmt_view *v,int initial,int selected,int previous){choices(v,true,false,initial,selected,previous);}
void menu_threshold_hint(const dmt_view *v,bool compact,bool english,int selected,unsigned row)
{
 static const uint16_t offsets[][6]={{0,27,77,105,155,183},{0,21,65,87,129,152}};
 unsigned n=row>=1 && row<=6?row-1:0;uint16_t off=offsets[compact][n];uint16_t ink=selected?(compact?0x1c9f:0xfd40):0xffff,fill=selected?ink:0x4228,text=selected?0:0xffff;
 if(row>=1 && row<=6){uint16_t x=compact?(row<5?281:279):(row<5?286:284);uint16_t y=(uint16_t)(off+(compact?43:30));view_measurement_char(v,x,y,row<3?'V':row<5?'A':'Z',16,ink);}
 if(!compact){
  view_fill(v,140,(uint16_t)(off+27),192,(uint16_t)(off+48),fill);
  for(unsigned i=0;i<6;i++)view_line(v,(uint16_t)(140+i),(uint16_t)(off+27),(uint16_t)(140+i),(uint16_t)(off+33-i),0);
  view_pixel(v,146,(uint16_t)(off+27),0);
  view_line(v,193,(uint16_t)(off+27),309,(uint16_t)(off+27),fill);view_line(v,309,(uint16_t)(off+27),309,(uint16_t)(off+41),fill);view_line(v,309,(uint16_t)(off+41),302,(uint16_t)(off+48),fill);view_line(v,193,(uint16_t)(off+48),302,(uint16_t)(off+48),fill);
  view_chinese(v,143,(uint16_t)(off+31),rom((row&0xfd)==1 || row==5?0x25460:0x25468),16,text);
 }else{
  view_fill(v,166,(uint16_t)(off+41),206,(uint16_t)(off+60),fill);
  for(unsigned i=0;i<4;i++)view_line(v,(uint16_t)(169-i),(uint16_t)(off+41),166,(uint16_t)(off+44-i),0);
  view_line(v,207,(uint16_t)(off+41),307,(uint16_t)(off+41),fill);view_line(v,207,(uint16_t)(off+60),304,(uint16_t)(off+60),fill);view_line(v,307,(uint16_t)(off+42),307,(uint16_t)(off+57),fill);view_line(v,304,(uint16_t)(off+60),307,(uint16_t)(off+57),fill);
  bool low=(row&0xfd)==1 || row==5;if(english)view_ascii(v,174,(uint16_t)(off+43),low?"MIN":"MAX",text,text,16);else view_chinese(v,167,(uint16_t)(off+45),rom(low?0x25460:0x25468),12,text);
 }
}
