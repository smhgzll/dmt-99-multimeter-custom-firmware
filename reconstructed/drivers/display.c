/* 0x7e2e,0x7e38,0x7e68,0x7f00,0x7f18,0x86f6,0x876a,0x8a02.
 * Panel reset/GPIO clocks are provided by the SDK port; this module owns the
 * recovered command stream and RGB565 geometry, without guessing a panel ID.
 */
#include "display.h"
static void command(const dmt_display *d,uint16_t v){display_write_command(&d->bus,v);}
static void data(const dmt_display *d,uint16_t v){display_write_data(&d->bus,v);}
static void coordinate(const dmt_display *d,uint16_t v){data(d,v>>8);data(d,v&255);}
void display_cursor(const dmt_display *d,uint16_t x,uint16_t y)
{command(d,d->column_command);coordinate(d,x);command(d,d->row_command);coordinate(d,y);}
void display_window(const dmt_display *d,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    command(d,d->column_command);coordinate(d,x);coordinate(d,(uint16_t)(x+width-1));
    command(d,d->row_command);coordinate(d,y);coordinate(d,(uint16_t)(y+height-1));
}
void display_orientation(dmt_display *d,bool landscape)
{
    d->width=landscape?320:240;d->height=landscape?240:320;d->orientation=landscape?3:5;
    d->column_command=0x2a;d->row_command=0x2b;d->pixels_command=0x2c;
    display_scan_direction(d,d->orientation);
}
void display_scan_direction(const dmt_display *d,uint32_t direction)
{
    static const uint8_t values[]={0xa0,0xe0,0x20,0x60,0x80,0,0xc0,0x40};
    command(d,0x36);data(d,direction<8?values[direction]:0);
    command(d,d->column_command);data(d,0);data(d,0);
    data(d,d->width?(d->width-1)>>8:0xffff);data(d,(uint16_t)(d->width-1)&255);
    command(d,d->row_command);data(d,0);data(d,0);
    data(d,d->height?(d->height-1)>>8:0xffff);data(d,(uint16_t)(d->height-1)&255);
}
void display_pixel(void *v,uint16_t x,uint16_t y,uint16_t color)
{dmt_display *d=v;display_cursor(d,x,y);command(d,d->pixels_command);data(d,color);}
void display_solid(const dmt_display *d,uint16_t color)
{
    display_window(d,0,0,320,240);display_cursor(d,0,0);command(d,d->pixels_command);
    for(uint32_t n=0;n<(uint32_t)d->width*d->height;++n)data(d,color);
}
void display_rectangle_fill(const dmt_display *d,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{
    display_window(d,0,0,320,240);
    for(uint32_t yy=y;yy<=y1;++yy){display_cursor(d,x,yy);command(d,d->pixels_command);
        for(uint32_t i=0;i<(uint16_t)(x1-x+1);++i)data(d,color);
    }
}
void display_panel_sequence(dmt_display *d,void (*delay)(void *,uint32_t),void *ctx)
{
    /* Literal command/data groups in 0x87aa, following PC3 reset timing. */
    static const uint8_t sequence[]={0x36,1,0,0x3a,1,5,0xb2,5,12,12,0,0x33,0x33,0xb7,1,0x35,0xbb,1,0x28,0xc0,1,0x2c,0xc2,1,1,0xc3,1,11,0xc4,1,0x20,0xc6,1,15,0xd0,2,0xa4,0xa1,
       0xe0,14,0xd0,1,8,15,17,0x2a,0x36,0x55,0x44,0x3a,11,6,17,0x20,
       0xe1,14,0xd0,2,7,10,11,0x18,0x34,0x43,0x4a,0x2b,0x1b,0x1c,0x22,0x1f};
    command(d,0x11);delay(ctx,120);
    for(unsigned i=0;i<sizeof sequence;){command(d,sequence[i++]);unsigned n=sequence[i++];while(n--)data(d,sequence[i++]);}
    command(d,0x29);command(d,0x2c);display_orientation(d,true);display_solid(d,0);
}
