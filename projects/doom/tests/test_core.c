/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "app.h"
#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static uint16_t pixels[320*240];
static unsigned writes,max_transfer;
static void write_pixels(void *c,unsigned x,unsigned y,unsigned w,unsigned h,const uint16_t *p)
{
    (void)c;assert(w && h && x<320 && y<240 && w<=320-x && h<=240-y);
    assert(w*h<=800);
    for(unsigned yy=0;yy<h;++yy)memcpy(pixels+(y+yy)*320+x,p+yy*w,w*2);
    ++writes;if(w*h>max_transfer)max_transfer=w*h;
}
static const doom_screen screen={0,write_pixels};
static void rays_equal(void)
{
    doom_ray rays[DOOM_RAYS];doom_cast(rays);
    doom_state s=doom_get_state();
    float a=s.angle*3.14159265f/180,ca=cosf(a),sa=sinf(a);
    for(unsigned i=0;i<DOOM_RAYS;++i){
        float side=(1.0f-2.0f*i/(DOOM_RAYS-1))*0.577350269f;
        doom_ray all=doom_cast_at(s.x,s.y,ca-sa*side,sa+ca*side,false);
        assert(rays[i].wall==all.wall);
        if(all.wall!=0xffff){assert(fabsf(rays[i].distance-fmaxf(1,all.distance))<0.001f);assert(fabsf(rays[i].fraction-all.fraction)<0.0001f);}
    }
}
int main(void)
{
    doom_app a;doom_app_init(&a);
    assert(a.mode==DOOM_MENU && !a.has_game);
    doom_app_draw(&a,&screen);assert(!a.dirty);
    doom_app_input(&a,'s');assert(a.selected==1);
    doom_app_input(&a,'f');assert(a.mode==DOOM_WEAPONS);
    doom_app_input(&a,'s');doom_app_input(&a,'f');assert(doom_get_state().weapon==DOOM_SHOTGUN);
    doom_app_input(&a,'s');doom_app_input(&a,'f');assert(a.mode==DOOM_MENU && doom_god_enabled());
    doom_app_input(&a,'f');assert(!doom_god_enabled());
    doom_app_input(&a,'s');doom_app_input(&a,'f');assert(!a.sound_enabled);
    doom_app_input(&a,'s');doom_app_input(&a,'f');assert(a.mode==DOOM_HELP);
    doom_app_draw(&a,&screen);doom_app_input(&a,'p');assert(a.mode==DOOM_MENU);
    doom_app_input(&a,'s');doom_app_input(&a,'f');assert(a.mode==DOOM_GAME);
    doom_state initial=doom_get_state();assert(initial.x==96 && initial.y== -272 && initial.angle==0 && initial.health==100);
    assert(!doom_collision(initial.x,initial.y));
    doom_app_input(&a,'w');doom_state moved=doom_get_state();assert(moved.x>initial.x);
    doom_app_input(&a,'p');assert(a.mode==DOOM_MENU);
    doom_app_input(&a,'f');assert(a.mode==DOOM_GAME && doom_get_state().x==moved.x);
    doom_app_input(&a,'p');doom_app_input(&a,'s');doom_app_input(&a,'f');
    assert(a.mode==DOOM_GAME && doom_get_state().x==initial.x);
    assert(!a.sound_enabled); /* session options survive a new game */
    doom_set_god(true);doom_new_game();assert(doom_god_enabled());
    doom_app_init(&a);assert(!doom_god_enabled() && a.sound_enabled);
    assert(!doom_action('q')); /* no persistent save or EEPROM */
    for(unsigned i=0;i<24;++i){rays_equal();doom_action('a');}
    doom_new_game();
    for(unsigned i=0;i<100;++i)doom_action('w');
    assert(!doom_collision(doom_get_state().x,doom_get_state().y));
    /* Bounded deterministic walk/combat plus rendering at many poses. */
    uint32_t random=12345;
    for(unsigned i=0;i<2000;++i){
        random=random*1664525u+1013904223u;
        doom_action("wasdf"[(random>>16)%5]);doom_advance(100);
        doom_state s=doom_get_state();assert(isfinite(s.x) && isfinite(s.y) && s.angle>=0 && s.angle<360 && s.health>=0 && s.health<=100);
        if(i%40==0){rays_equal();doom_render(&screen);}
    }
    doom_new_game();memset(pixels,0x55,sizeof pixels);doom_render(&screen);
    for(unsigned i=0;i<320*240;++i)assert(pixels[i]!=0x5555);
    assert(writes>100 && max_transfer==800);
    printf("PASS: menu/resume/new game, collision, 5920 filtered/full ray comparisons, 2000 actions, render bounds; max transfer %u pixels\n",max_transfer);
    return 0;
}
