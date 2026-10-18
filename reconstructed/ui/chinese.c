/* Shared native form of0x8a5a,0x8b5c,0x8c64,0x8d6c and string loop0x8e46.
 * Each record contains a two-byte character and row-major LSB-first pixels.
 * The original opaque path sets a window with end-like values passed as sizes
 * and streams data without an added memory-write command. Preserve that wire
 * behavior, independently of how a physical panel might interpret it.
 */
#include "chinese.h"
void display_chinese_glyph(const dmt_display *d,uint16_t x,uint16_t y,uint16_t code,
                          uint16_t fg,uint16_t bg,uint8_t size,bool transparent)
{
    unsigned count,stride,offset;
    switch(size){
    case 12:count=12;stride=26;offset=0;break;
    case 16:count=63;stride=34;offset=0x21ef4-0x21dbc;break;
    case 24:count=49;stride=74;offset=0x22754-0x21dbc;break;
    case 32:count=1;stride=130;offset=0x23580-0x21dbc;break;
    default:return;
    }
    uint16_t xx=x,yy=y;unsigned row_bit=0;
    for(unsigned record=0;record<count;++record){
        const uint8_t *p=dmt_chinese_rom+offset+record*stride;
        if(code!=((uint16_t)p[0]<<8|p[1]))continue;
        display_window(d,xx,yy,(uint16_t)(size-1+xx),(uint16_t)(size-1+yy));
        unsigned bytes=size*((size+7)/8);
        for(unsigned i=0;i<bytes;++i){
            uint8_t bits=p[i+2];
            for(unsigned bit=0;bit<8;++bit){
                bool set=(bits&(1u<<bit))!=0;
                if(!transparent){
                    display_write_data(&d->bus,set?fg:bg);
                    if(++row_bit%size==0){row_bit=0;break;}
                }else{
                    if(set)display_pixel((void *)d,xx,yy,fg);
                    xx=(uint16_t)(xx+1);
                    if((uint16_t)(xx-x)==size){yy=(uint16_t)(yy+1);xx=x;break;}
                }
            }
        }
    }
}
void display_chinese_string(const dmt_display *d,uint16_t x,uint16_t y,const uint8_t *p,
                           uint16_t fg,uint16_t bg,uint8_t size,bool transparent)
{
    if(size!=12 && size!=16 && size!=24 && size!=32)return;
    while(*p){display_chinese_glyph(d,x,y,((uint16_t)p[0]<<8)|p[1],fg,bg,size,transparent);p+=2;x=(uint16_t)(x+size);}
}
