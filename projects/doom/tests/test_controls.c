/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "controls.h"
#include <assert.h>
#include <stdio.h>
static doom_controls c;
static uint32_t now;
static unsigned fire,pause;
static int32_t move,turn;
static doom_control_event poll(uint8_t buttons,uint32_t dt,bool game)
{
    now+=dt;doom_control_event e=doom_controls_poll(&c,buttons,now,game);
    if(e.key=='f')++fire;
    if(e.key=='p')++pause;
    move+=e.move_q8;turn+=e.turn_q8;return e;
}
static void reset(uint8_t down,uint32_t time)
{now=time;doom_controls_init(&c,down,now);fire=pause=0;move=turn=0;}
static void hold(uint8_t down,unsigned ms,bool game)
{for(unsigned n=0;n<ms/10;++n)poll(down,10,game);}
int main(void)
{
    reset(0,0);hold(DB_FIRE,100,true);assert(fire==1 && !pause);
    hold(DB_FIRE,2900,true);assert(fire>10 && !pause && !move && !turn);
    hold(0,30,true);unsigned before=fire;hold(DB_FIRE,30,true);assert(fire==before+1);
    reset(0,0);hold(DB_FIRE|DB_DOWN,2020,true);assert(!fire && !pause && !move);
    hold(DB_FIRE|DB_DOWN,10,true);assert(pause==1 && !fire);
    hold(DB_FIRE|DB_DOWN,3000,false);assert(pause==1 && !move);
    hold(DB_FIRE,100,false);assert(!fire);hold(0,40,false);hold(DB_FIRE,30,false);assert(fire==1);
    reset(0,0);hold(DB_FIRE|DB_DOWN,1500,true);hold(0,100,true);assert(!pause && !fire && !move);
    /* Menus get one confirmation per physical press, no fire repeat. */
    reset(0,0);hold(DB_FIRE,3000,false);assert(fire==1 && !pause);
    reset(DB_FIRE|DB_POWER,0);hold(DB_FIRE|DB_POWER,3000,true);assert(!fire && !pause);
    hold(0,40,true);assert(!poll(DB_POWER,10,true).power);assert(poll(DB_POWER,20,true).power);
    reset(0,0);hold(DB_UP|DB_LEFT,100,true);
    assert(move>0 && move<5*256 && turn>0 && turn<3*256);
    int32_t slow_move=move,slow_turn=turn;hold(DB_UP|DB_LEFT,1000,true);
    before=(unsigned)move;int32_t t=turn;hold(DB_UP|DB_LEFT,100,true);
    assert(move-(int32_t)before>slow_move*3 && turn-t>slow_turn*3);
    before=(unsigned)move;t=turn;hold(DB_UP|DB_LEFT,1000,true);
    assert(move-(int32_t)before==160*256 && turn-t==80*256);
    before=(unsigned)move;t=turn;hold(0,100,true);assert(move==(int32_t)before && turn==t);
    hold(DB_DOWN|DB_RIGHT,100,true);assert(move<(int32_t)before && turn<t);
    assert((int32_t)before-move<5*256 && t-turn<3*256); /* direction resets ramp */
    before=(unsigned)move;t=turn;hold(DB_UP|DB_DOWN|DB_LEFT|DB_RIGHT,100,true);
    assert(move==(int32_t)before && turn==t); /* opposed keys cancel */
    reset(0,UINT32_MAX-1000);hold(DB_FIRE|DB_DOWN,2030,true);assert(pause==1 && !fire);
    puts("PASS: fire on press/repeat, no solo pause, exact 2s chord, debounce/release, boot guards, wrap, smooth ramps and bounded top speeds");
}
