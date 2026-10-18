/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "app.h"
#include "game.h"
#include "controls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Desktop only; never linked in the CH32 image. */
static uint16_t framebuffer[320*240];
static doom_app app;
static doom_controls controls;
static uint8_t buttons;
static uint32_t input_clock;
static void write_pixels(void *c,unsigned x,unsigned y,unsigned w,unsigned h,const uint16_t *p)
{
    (void)c;
    if(!w || !h || x>=320 || y>=240 || w>320-x || h>240-y)abort();
    for(unsigned yy=0;yy<h;++yy)memcpy(framebuffer+(y+yy)*320+x,p+yy*w,w*sizeof *p);
}
static const doom_screen screen={0,write_pixels};
void doom_host_init(void){doom_app_init(&app);buttons=0;input_clock=0;doom_controls_init(&controls,0,0);doom_app_draw(&app,&screen);}
void doom_host_input(char key){doom_app_input(&app,key);if(app.dirty)doom_app_draw(&app,&screen);}
static void physical_input(void)
{
    doom_control_event e=doom_controls_poll(&controls,buttons,input_clock,app.mode==DOOM_GAME);
    if(e.key)doom_app_input(&app,e.key);
    doom_app_motion(&app,e.move_q8,e.turn_q8);
}
void doom_host_buttons(uint8_t down){buttons=down;physical_input();}
void doom_host_motion(int32_t move,int32_t turn){doom_app_motion(&app,move,turn);if(app.dirty)doom_app_draw(&app,&screen);}
void doom_host_advance(uint32_t ms)
{
    if(ms>500)ms=500;
    while(ms){uint32_t dt=ms>10?10:ms;input_clock+=dt;physical_input();doom_app_update(&app,dt);ms-=dt;}
    if(app.dirty)doom_app_draw(&app,&screen);
}
const uint16_t *doom_host_pixels(void){return framebuffer;}
int doom_host_mode(void){return (int)app.mode;}

#ifndef DOOM_HOST_LIBRARY
static bool ppm(const char *path)
{
    FILE *f=fopen(path,"wb");if(!f)return false;
    fprintf(f,"P6\n320 240\n255\n");
    for(unsigned i=0;i<320*240;++i){
        uint16_t c=framebuffer[i];
        unsigned char rgb[3]={(unsigned char)((c>>11)*255/31),
            (unsigned char)(((c>>5)&63)*255/63),(unsigned char)((c&31)*255/31)};
        if(fwrite(rgb,1,3,f)!=3){fclose(f);return false;}
    }
    return fclose(f)==0;
}
int main(int argc,char **argv)
{
    const char *script="f",*image="build/doom.ppm",*json=0;
    for(int i=1;i<argc;++i){
        if(!strcmp(argv[i],"--script") && i+1<argc)script=argv[++i];
        else if(!strcmp(argv[i],"--ppm") && i+1<argc)image=argv[++i];
        else if(!strcmp(argv[i],"--state") && i+1<argc)json=argv[++i];
        else {fprintf(stderr,"usage: doom-host [--script fwwadf] [--ppm image.ppm] [--state state.json]\n");return 2;}
    }
    doom_host_init();for(const char *p=script;*p;++p){if(*p=='.')doom_host_advance(100);else doom_host_input(*p);}
    if(!ppm(image)){perror(image);return 1;}
    doom_state s=doom_get_state();FILE *f=json?fopen(json,"w"):stdout;
    if(!f){perror(json);return 1;}
    fprintf(f,"{\"x\":%.6f,\"y\":%.6f,\"angle\":%d,\"health\":%d,\"kills\":%u,\"pickups\":%u,\"mode\":%u,\"weapon\":%u,\"tick\":%u,\"god\":%s,\"sound\":%s}\n",
            (double)s.x,(double)s.y,s.angle,s.health,s.kills,s.pickups,(unsigned)app.mode,(unsigned)s.weapon,doom_get_tick(),doom_god_enabled()?"true":"false",app.sound_enabled?"true":"false");
    if(json && fclose(f))return 1;
    return 0;
}
#endif
