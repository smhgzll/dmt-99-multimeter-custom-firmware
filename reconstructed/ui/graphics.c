/* Native drawing algorithms recovered from 0x8050..0x86f6 and 0x1a360.
 * Pixel order, inclusive bounds, the line algorithm's extra endpoint sample,
 * and the small-font special glyphs are deliberate original behavior.
 */
#include "graphics.h"
static void pixel(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t color)
{c->pixel(c->context,x,y,color);}
void graphics_line(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{
    int dx=(int)x1-x,dy=(int)y1-y,sx=dx>0?1:dx<0?-1:0,sy=dy>0?1:dy<0?-1:0;
    if(dx<0)dx=-dx;
    if(dy<0)dy=-dy;
    int distance=dx>dy?dx:dy,ex=0,ey=0;
    for(int i=0;i<=distance+1;++i){
        ex+=dx;pixel(c,x,y,color);ey+=dy;
        if(ex>distance){ex-=distance;x=(uint16_t)(x+sx);}
        if(ey>distance){ey-=distance;y=(uint16_t)(y+sy);}
    }
}
void graphics_rectangle(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{
    graphics_line(c,x,y,x1,y,color);graphics_line(c,x,y,x,y1,color);
    graphics_line(c,x,y1,x1,y1,color);graphics_line(c,x1,y,x1,y1,color);
}
void graphics_fill(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{
    if(c->fill && x<=x1 && y<=y1){c->fill(c->context,x,y,x1,y1,color);return;}
    /* Bounded loops replace the original possible uint16 wrap at 65535. */
    for(uint32_t yy=y;yy<=y1;++yy)
        for(uint32_t n=0;n<(uint16_t)(x1-x+1);++n)pixel(c,(uint16_t)(x+n),(uint16_t)yy,color);
}
void graphics_cut_fill(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint8_t cut,uint16_t color)
{
    graphics_fill(c,x,y,x1,y1,color);
    for(unsigned i=0;i<cut;++i){
        uint16_t upper=(uint16_t)(cut-1+y-i),lower=(uint16_t)(y1-cut+1+i);
        graphics_line(c,x+i,y,x+i,upper,0);graphics_line(c,x+i,lower,x+i,y1,0);
        graphics_line(c,x1-i,y,x1-i,upper,0);graphics_line(c,x1-i,lower,x1-i,y1,0);
    }
}
void graphics_cut_border(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint8_t cut,uint16_t color)
{
    uint16_t left=x+cut,right=x1-cut,top=y+cut,bottom=y1-cut;
    graphics_line(c,left,y,right,y,color);graphics_line(c,x,top,x,bottom,color);
    graphics_line(c,left,y1,right,y1,color);graphics_line(c,x1,top,x1,bottom,color);
    graphics_line(c,left,y,x,top,color);graphics_line(c,right,y,x1,top,color);
    graphics_line(c,x,bottom,left,y1,color);graphics_line(c,right,y1,x1,bottom,color);
}
static const uint8_t *glyph(uint8_t ch,uint8_t size,size_t *bytes)
{
    uint8_t index=(uint8_t)(ch-32);if(index>=95)return 0;
    unsigned stride,offset;
    switch(size){case 12:stride=12;offset=0;break;case 16:stride=16;offset=0x474;break;
    case 24:stride=36;offset=0xae4;break;case 32:stride=64;offset=0x1840;break;default:return 0;}
    *bytes=stride;return dmt_font_rom+offset+index*stride;
}
static void glyph_pixels(const dmt_canvas *c,uint16_t x,uint16_t y,const uint8_t *p,size_t n,uint8_t height,uint16_t fg,uint16_t bg,bool transparent)
{
    uint16_t yy=y;
    for(size_t i=0;i<n;++i){
        uint8_t bits=p[i];
        for(unsigned bit=0;bit<8;++bit){
            if((bits&0x80) || !transparent)pixel(c,x,yy,(bits&0x80)?fg:bg);
            yy=(uint16_t)(yy+1);bits<<=1;
            if(yy>=c->height)return;
            if((uint16_t)(yy-y)==height){x=(uint16_t)(x+1);yy=y;if(x>=c->width)return;break;}
        }
    }
}
void graphics_ascii(const dmt_canvas *c,uint16_t x,uint16_t y,uint8_t ch,uint8_t size,uint16_t fg,uint16_t bg,bool transparent)
{size_t n;const uint8_t *p=glyph(ch,size,&n);if(p)glyph_pixels(c,x,y,p,n,size,fg,bg,transparent);}
void graphics_measurement_char(const dmt_canvas *c,uint16_t x,uint16_t y,uint8_t ch,uint8_t size,uint16_t color,bool transparent)
{
    size_t n;const uint8_t *p=glyph(ch,size,&n);if(!p)return;
    if(size==16 && ((ch>='X' && ch<='Z') || ch=='J')){
        unsigned special=ch=='J'?3:ch-'X';p=dmt_font_rom+(0x1f820-0x1edbc)+special*32;n=32;
    }else if(size==32 && ch=='.')n/=2;
    glyph_pixels(c,x,y,p,n,size,color,0,transparent);
}
void graphics_ascii_text(const dmt_canvas *c,uint16_t x,uint16_t y,const char *p,uint8_t size,uint16_t fg,uint16_t bg,bool transparent)
{for(;(uint8_t)(*p-32)<95;++p){graphics_ascii(c,x,y,(uint8_t)*p,size,fg,bg,transparent);x=(uint16_t)(x+(size>>1));}}
void graphics_text_box(const dmt_canvas *c,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint16_t color,const char *p,bool special_width)
{
    uint16_t right=x+width,bottom=y+height;uint8_t left=(uint8_t)x;
    for(;(uint8_t)(*p-32)<95;++p){
        if(x>=right){y=(uint16_t)(y+size);x=left;}
        if(y>=bottom)return;
        graphics_measurement_char(c,x,y,(uint8_t)*p,size,color,false);
        x=(uint16_t)(x+(*p=='.'?(size>>2):(size>>1)));
        if(special_width && size==16 && ((*p>='X' && *p<='Z') || *p=='J'))x=(uint16_t)(x+6);
    }
}
void graphics_number(const dmt_canvas *c,uint16_t x,uint16_t y,uint32_t value,uint8_t digits,uint8_t size,uint8_t flags,uint16_t color)
{
    bool seen=false;
    /* All recovered call sites use <=10 decimal positions. */
    if(digits>10)return;
    for(unsigned i=0;i<digits;++i){
        uint32_t scale=1;for(unsigned j=i+1;j<digits;++j)scale*=10;
        uint8_t digit=(value/scale)%10;char ch;
        if(seen || i+1>=digits || digit){seen=true;ch=(char)('0'+digit);}else ch=(flags&0x80)?'0':' ';
        graphics_measurement_char(c,x,y,(uint8_t)ch,size,color,(flags&1)!=0);x=(uint16_t)(x+(size>>1));
    }
}
void graphics_bitmap(const dmt_canvas *c,uint16_t x,uint16_t y,const uint8_t *data,size_t length,uint16_t fg,uint16_t bg)
{
    if(length<6)return;
    unsigned width=data[2]|(data[3]<<8),height=data[4]|(data[5]<<8),stride=(width+7)/8;
    if(height>255 || width>2047 || (size_t)stride*height>length-6)return;
    for(unsigned yy=0;yy<height;++yy)
        for(unsigned xx=0;xx<width;++xx)pixel(c,(uint16_t)(x+xx),(uint16_t)(y+yy),(data[6+yy*stride+xx/8]&(0x80>>(xx%8)))?fg:bg);
}
