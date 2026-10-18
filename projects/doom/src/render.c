/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Local RGB565 renderer. Four physical columns per ray; no target framebuffer.
 */
#include "render.h"
#include "game.h"
#include "assets.h"
#include <stddef.h>

static doom_ray rays[DOOM_RAYS];
static doom_sprite visible[DOOM_MAX_SPRITES];
static uint16_t strip[4*DOOM_VIEW_HEIGHT];
static uint16_t row[320];

static uint16_t dim(uint16_t c,unsigned light)
{
    return (uint16_t)((((c>>11)*light>>8)<<11) |
           ((((c>>5)&63)*light>>8)<<5) | ((c&31)*light>>8));
}
static unsigned light_at(float distance)
{int v=256-(int)(distance*0.08f);return v<90?90u:(unsigned)v;}
void doom_fill(const doom_screen *s,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t c)
{
    if(x>=320 || y>=240)return;
    if(w>320-x)w=320-x;
    if(h>240-y)h=240-y;
    for(unsigned i=0;i<w;++i)row[i]=c;
    for(unsigned j=0;j<h;++j)s->write(s->context,x,y+j,w,1,row);
}
/* Original small 3x5 font, row bit 2 at the left. */
static const char chars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-+/? .";
static const uint8_t font[][5]={
 {2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},{7,4,6,4,7},
 {7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},{7,2,2,2,7},{1,1,1,5,2},
 {5,5,6,5,5},{4,4,4,4,7},{5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},
 {6,5,6,4,4},{2,5,5,7,3},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},
 {5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},{5,5,2,2,2},
 {7,1,2,4,7},{7,5,5,5,7},{2,6,2,2,7},{6,1,2,4,7},{6,1,2,1,6},
 {5,5,7,1,1},{7,4,6,1,6},{3,4,6,5,2},{7,1,2,2,2},{2,5,2,5,2},
 {2,5,3,1,6},{0,2,0,2,0},{0,0,7,0,0},{0,2,7,2,0},{1,1,2,4,4},
 {6,1,2,0,2},{0,0,0,0,0},{0,0,0,0,2}};
void doom_text(const doom_screen *s,unsigned x,unsigned y,const char *text,uint16_t fg,uint16_t bg,unsigned scale)
{
    uint16_t glyph[12*15];
    if(scale<1 || scale>3)return;
    unsigned w=4*scale,h=5*scale;
    for(;*text && x+w<=320 && y+h<=240;++text,x+=w){
        unsigned k=0;while(chars[k] && chars[k]!=*text)++k;
        if(!chars[k])k=39;
        for(unsigned yy=0;yy<h;++yy)for(unsigned xx=0;xx<w;++xx)
            glyph[yy*w+xx]=(xx/scale<3 && (font[k][yy/scale]&(4u>>(xx/scale))))?fg:bg;
        s->write(s->context,x,y,w,h,glyph);
    }
}
static void number(const doom_screen *s,unsigned x,unsigned y,unsigned value,uint16_t fg)
{
    char str[6];unsigned n=0;
    do{str[n++]=(char)('0'+value%10);value/=10;}while(value && n<5);
    for(unsigned i=0;i<n/2;++i){char c=str[i];str[i]=str[n-1-i];str[n-1-i]=c;}str[n]=0;
    doom_text(s,x,y,str,fg,0x2104,2);
}
static bool opaque(unsigned sprite,unsigned x,unsigned y)
{unsigned bit=y*32+x;return (doom_masks[sprite*SPRITE_MASK_BYTES+bit/8]>>(bit%8))&1;}
void doom_render(const doom_screen *s)
{
    doom_state state=doom_get_state();
    doom_cast(rays);
    unsigned sprites=doom_project_sprites(visible,DOOM_MAX_SPRITES);
    for(unsigned col=0;col<DOOM_RAYS;++col){
        doom_ray r=rays[col];
        int height=r.wall==0xffff?0:(int)(200.0f*64.0f/r.distance);
        if(height>600)height=600;
        if(height<1 && r.wall!=0xffff)height=1;
        int top=100-height/2;
        unsigned tx=(unsigned)(r.fraction*31.999f);
        if(tx>31)tx=31;
        unsigned light=light_at(r.distance);
        for(int y=0;y<200;++y){
            uint16_t color=y<100?0x18c3:0x2944;
            if(height && y>=top && y<top+height){
                unsigned ty=(unsigned)((y-top)*32/height);
                unsigned tex=doom_wall_texture[r.wall];
                color=dim(doom_palette[doom_textures[tex*1024+ty*32+tx]],light);
            }
            for(unsigned x=0;x<4;++x)strip[y*4+x]=color;
        }
        for(unsigned i=0;i<sprites;++i){
            doom_sprite sp=visible[i];
            if(r.wall!=0xffff && sp.depth>r.distance)continue;
            int sh=(int)(200.0f*sp.world_h/sp.depth);if(sh>240)sh=240;if(sh<2)sh=2;
            int sw=(int)(200.0f*sp.world_w/sp.depth);if(sw>320)sw=320;if(sw<1)sw=1;
            int left=sp.center-sw/2,bottom=100+(int)(200.0f*(32-sp.elevation)/sp.depth),st=bottom-sh;
            int y0=st<0?0:st,y1=bottom>200?200:bottom;
            unsigned sl=light_at(sp.depth);
            for(unsigned x=0;x<4;++x){
                int sx=(int)(col*4+x)-left;if(sx<0 || sx>=sw)continue;
                for(int y=y0;y<y1;++y){
                    unsigned u=(unsigned)(sx*32/sw),v=(unsigned)((y-st)*SPRITE_H/sh);
                    if(sp.sprite<SPRITE_COUNT){
                        if(opaque(sp.sprite,u,v))strip[y*4+x]=dim(doom_palette[doom_sprites[sp.sprite*SPRITE_PIXELS+v*32+u]],sl);
                    }else strip[y*4+x]=0xffe0;
                }
            }
        }
        uint8_t ws=doom_weapon_sprite();
        int wh=state.weapon==DOOM_SAW?60:state.weapon==DOOM_SHOTGUN?78:80;
        int ww=(int)(wh*doom_sprite_width[ws]/doom_sprite_height[ws]);if(ww>136)ww=136;
        int bob=doom_weapon_bob(),left=160-ww/2+bob,weapon_top=200-wh+(bob<0?-bob:bob)+(state.shot?4:0);
        for(unsigned x=0;x<4;++x){
            int sx=(int)(col*4+x)-left;if(sx<0 || sx>=ww)continue;
            unsigned u=(unsigned)(sx*32/ww);
            for(int y=weapon_top;y<200;++y){
                unsigned v=(unsigned)((y-weapon_top)*SPRITE_H/wh);
                if(v<SPRITE_H && opaque(ws,u,v))strip[y*4+x]=doom_palette[doom_sprites[ws*SPRITE_PIXELS+v*32+u]];
            }
        }
        s->write(s->context,col*4,0,4,200,strip);
    }
    doom_fill(s,159,97,2,6,0xffff);doom_fill(s,157,99,6,2,0xffff);
    if(state.shot && state.weapon!=DOOM_FIST && state.weapon!=DOOM_SAW){
        doom_fill(s,156,128,8,20,0xffe0);doom_fill(s,150,134,20,6,0xffff);
    }
    doom_fill(s,0,200,320,40,0x2104);
    doom_text(s,8,205,"HEALTH",0xf800,0x2104,2);number(s,64,205,(unsigned)state.health,0xffff);
    doom_text(s,106,205,"KILLS",0xffe0,0x2104,2);number(s,154,205,state.kills,0xffff);
    doom_text(s,198,205,"ITEMS",0x07e0,0x2104,2);number(s,246,205,state.pickups,0xffff);
    doom_text(s,8,224,doom_weapon_details(state.weapon)->name,0xffe0,0x2104,2);
    doom_text(s,128,224,"C+DOWN 2S: MENU",0xc618,0x2104,2);
    if(doom_god_enabled())doom_text(s,294,205,"GOD",0x07e0,0x2104,1);
    if(state.complete){
        doom_fill(s,28,70,264,38,0);
        doom_text(s,52,82,"LEVEL COMPLETE",0x07e0,0,3);
    }
}
void doom_menu(const doom_screen *s,unsigned selected,bool paused,bool god,bool sound)
{
    doom_fill(s,0,0,320,240,0);
    for(unsigned y=0;y<40;++y){
        for(unsigned x=0;x<128;++x)row[x*2]=row[x*2+1]=doom_palette[doom_logo[y*128+x]];
        s->write(s->context,32,12+y*2,256,1,row);
        s->write(s->context,32,13+y*2,256,1,row);
    }
    doom_text(s,76,100,"POTAT-99 DOOM",0xc618,0,2);
    const char *labels[6]={"RESUME","NEW GAME","WEAPONS",god?"GOD MODE: ON":"GOD MODE: OFF",sound?"SOUND: ON":"SOUND: OFF","CONTROLS"};
    unsigned count=paused?6:5,offset=paused?0:1;
    for(unsigned i=0;i<count;++i){
        doom_text(s,52,122+i*16,selected==i?"+":" ",0xf800,0,2);
        doom_text(s,76,122+i*16,labels[i+offset],selected==i?0xffe0:0xc618,0,2);
    }
    doom_text(s,24,222,"UP/DOWN: SELECT  PC8: OK",0x8410,0,2);
}

void doom_weapon_menu(const doom_screen *s,unsigned selected)
{
    doom_fill(s,0,0,320,240,0x1082);
    doom_text(s,92,18,"WEAPONS",0xf800,0x1082,3);
    if(selected>=DOOM_WEAPON_COUNT)selected=DOOM_PISTOL;
    for(unsigned i=0;i<DOOM_WEAPON_COUNT;++i)
        doom_text(s,14,60+i*23,doom_weapon_details(i)->name,i==selected?0xffe0:0x8410,0x1082,2);
    unsigned sprite=doom_weapon_frames[selected*4];
    unsigned h=80,w=h*doom_sprite_width[sprite]/doom_sprite_height[sprite];if(w>136)w=136;
    for(unsigned y=0;y<h;++y){
        for(unsigned x=0;x<w;++x){unsigned u=x*32/w,v=y*SPRITE_H/h;
            row[x]=opaque(sprite,u,v)?doom_palette[doom_sprites[sprite*SPRITE_PIXELS+v*32+u]]:0x1082;}
        s->write(s->context,172+(136-w)/2,60+y,w,1,row);
    }
    doom_text(s,176,164,"DAMAGE",0xc618,0x1082,2);
    /* number() has a HUD background; use a small local decimal field here. */
    char value[4];const doom_weapon_info *info=doom_weapon_details(selected);
    unsigned n=info->damage*info->pellets*info->burst;
    value[0]=(char)('0'+n/100);value[1]=(char)('0'+n/10%10);value[2]=(char)('0'+n%10);value[3]=0;
    doom_text(s,232,164,value,0xffe0,0x1082,2);
    doom_text(s,16,218,"UP/DOWN  PC8: USE  LEFT: BACK",0xc618,0x1082,2);
}
