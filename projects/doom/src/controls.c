/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Debounced input and bounded, time-based movement ramp. No MMIO or heap.
 */
#include "controls.h"
void doom_controls_init(doom_controls *c,uint8_t down,uint32_t now)
{
    *c=(doom_controls){.previous=now};
    for(unsigned i=0;i<6;++i)c->keys[i]=(doom_key){.raw=!!(down&(1u<<i)),.stable=!!(down&(1u<<i)),.armed=!(down&(1u<<i)),.changed=now};
}
static int32_t axis(int dir,int8_t *old,int32_t *speed,int32_t *remainder,
                    uint32_t dt,int start,int maximum,int acceleration)
{
    if(!dir){*old=0;*speed=*remainder=0;return 0;}
    if(dir!=*old){*old=(int8_t)dir;*speed=start*256;*remainder=0;}
    int32_t before=*speed;
    *speed+=(int32_t)dt*acceleration*256/1000;
    if(*speed>maximum*256)*speed=maximum*256;
    /* Trapezoid integration, retaining sub-Q8 displacement between polls. */
    int32_t amount=(before+*speed)*(int32_t)dt/2+*remainder;
    *remainder=amount%1000;
    return dir*(amount/1000);
}
doom_control_event doom_controls_poll(doom_controls *c,uint8_t down,uint32_t now,bool game)
{
    doom_control_event out={0};uint8_t pressed=0,held=0;
    uint32_t dt=now-c->previous;c->previous=now;if(dt>100)dt=100;
    for(unsigned i=0;i<6;++i){
        doom_key *k=c->keys+i;bool raw=!!(down&(1u<<i));
        if(raw!=k->raw){k->raw=raw;k->changed=now;}
        if(!raw && !k->stable)k->armed=true;
        if(raw!=k->stable && now-k->changed>=20){
            k->stable=raw;
            if(raw && k->armed){pressed|=(uint8_t)(1u<<i);k->next=now+(i==4?180:350);}
        }
        if(raw && k->stable && k->armed)held|=(uint8_t)(1u<<i);
    }
    out.power=!!(pressed&DB_POWER);
    bool chord=(held&(DB_FIRE|DB_DOWN))==(DB_FIRE|DB_DOWN);
    if(chord){
        if(!c->chord){c->chord=true;c->chord_sent=false;c->chord_start=now;}
        if(!c->chord_sent && now-c->chord_start>=2000){out.key='p';c->chord_sent=true;}
    }else {c->chord=false;c->chord_sent=false;}
    /* Raw chord immediately suppresses firing/motion during debounce too.
     * A completed chord stays inhibited until BOTH buttons are released. */
    if((down&(DB_FIRE|DB_DOWN))==(DB_FIRE|DB_DOWN))c->chord_blocked=true;
    if(!(down&(DB_FIRE|DB_DOWN)))c->chord_blocked=false;
    if(c->chord_blocked){held&=(uint8_t)~(DB_FIRE|DB_DOWN);pressed&=(uint8_t)~(DB_FIRE|DB_DOWN);}
    static const char actions[]={'s','w','d','a','f'};
    if(!out.key){
        for(unsigned i=0;i<5;++i){
            uint8_t bit=(uint8_t)(1u<<i);
            if(game && i<4)continue;
            doom_key *k=c->keys+i;
            if((pressed&bit) || ((held&bit) && (game || i!=4) && (int32_t)(now-k->next)>=0)){
                out.key=actions[i];k->next=now+(i==4?180:150);break;
            }
        }
    }
    int move=game?!!(held&DB_UP)-!!(held&DB_DOWN):0;
    int turn=game?!!(held&DB_LEFT)-!!(held&DB_RIGHT):0;
    if((down&(DB_UP|DB_DOWN))==(DB_UP|DB_DOWN))move=0;
    if((down&(DB_LEFT|DB_RIGHT))==(DB_LEFT|DB_RIGHT))turn=0;
    if(chord){move=turn=0;}
    out.move_q8=axis(move,&c->move_dir,&c->move_speed,&c->move_remainder,dt,40,160,240);
    out.turn_q8=axis(turn,&c->turn_dir,&c->turn_speed,&c->turn_remainder,dt,16,80,128);
    return out;
}
