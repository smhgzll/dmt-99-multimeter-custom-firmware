#include "graph.h"
#include <string.h>
#include <limits.h>
void graph_strip_pixel(dmt_graph *g,uint32_t x,uint32_t y,uint16_t color)
{
 uint32_t row=(y-(uint32_t)g->strip*20)&65535;
 if(g->strip>9 || x>=196 || row>=20)return;
 if(g->strip==0 && y>=20)return;
 g->pixels[x*20+row]=color;
}
static void strip_pixel(void *g,uint16_t x,uint16_t y,uint16_t c){graph_strip_pixel(g,x,y,c);}
void graph_strip_line(dmt_graph *g,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{dmt_canvas c={g,strip_pixel,196,200,0};graphics_line(&c,x,y,x1,y1,color);}
void graph_strip_blit(const dmt_graph *g,const dmt_canvas *c,const dmt_display *d,uint16_t x,uint16_t y)
{
 if(d){display_window(d,x,y,196,20);display_write_command(&d->bus,d->pixels_command);}
 for(unsigned row=0;row<20;row++)for(unsigned col=0;col<196;col++){
  uint16_t p=g->pixels[col*20+row];
  if(d)display_write_data(&d->bus,p);
  else if(c && c->pixel)c->pixel(c->context,(uint16_t)(x+col),(uint16_t)(y+row),p);
 }
}
static uint16_t coordinate(double v)
{
 /* Original helper1d016 is unsigned: negative=>0, overflow/NaN=>UINT_MAX;
  * native cast is limited to its defined domain. Differential vectors cover it. */
 uint32_t n=v<0?0:!(v<4294967296.0)?UINT32_MAX:(uint32_t)v;
 uint16_t y=(uint16_t)n;return y>159?159:y;
}
void graph_update(dmt_graph *g,float value,uint8_t mode,uint8_t unit,uint32_t a1,uint32_t inherited,const dmt_canvas *c,const dmt_display *d)
{
 if(a1==1){memset(g->samples,0,sizeof g->samples);g->count=0;g->index=0;if(c)graphics_fill(c,37,53,239,216,0);}
 if(mode==0xb || mode==0xa || mode==0x11 || mode==0x16 || mode==0xf){
  unsigned n=(uint8_t)(unit-8);
  if(n<11 && ((0x489u>>n)&1))value=value/1000.0f;
  else if((((uint32_t)unit-13u) & 0xfbu)==0 || unit==20)value=value*1000.0f;
 }
 if(g->index==0)g->leading_sample=value;
 else if(g->index<=196)g->samples[g->index-1]=value;
 float maximum;memcpy(&maximum,&inherited,4);float minimum=9999.0f;
 for(unsigned i=0;i<196;i++)if(i<g->count){if(g->samples[i]>maximum)maximum=g->samples[i];if(g->samples[i]<minimum)minimum=g->samples[i];}
 float range=maximum-minimum;g->scale=160.0/(double)range;
 if(g->count)for(g->strip=0;g->strip<8;g->strip++){
  memset(g->pixels,0,sizeof g->pixels);
  for(unsigned x=0,i=g->index;x<195;x++,i++)if(x+g->count>193){
   float p=g->samples[i%196]-minimum,q=g->samples[(i+1)%196]-minimum;
   graph_strip_line(g,(uint16_t)x,coordinate(159.0-(double)p*g->scale),(uint16_t)(x+1),coordinate(159.0-(double)q*g->scale),0xffff);
  }
  graph_strip_blit(g,c,d,40,(uint16_t)(g->strip*20+55));
 }
 g->index=(uint16_t)((g->index+1)%197);g->count=(uint16_t)(g->count+1);
}
