/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Small nonblocking mono effects. No sample buffer, heap or interrupts.
 */
#include "sound.h"
#include "game.h"
typedef struct {uint16_t hz,ms;} note;
static const note walk[]={{800,24}},fire[]={{2600,35},{1300,35}},
    hurt[]={{330,100},{220,100}},kill[]={{1200,40},{1800,40},{2400,40}};
static const note *pattern(uint8_t event,unsigned *count)
{
    switch(event){
    case DOOM_STEP:*count=1;return walk;
    case DOOM_FIRE:*count=2;return fire;
    case DOOM_HURT:*count=2;return hurt;
    default:*count=3;return kill;
    }
}
void doom_sound_init(doom_sound *s,void *context,void (*tone)(void *,uint16_t))
{*s=(doom_sound){.context=context,.tone=tone};tone(context,0);}
void doom_sound_stop(doom_sound *s)
{s->active=s->pending=s->index=0;s->tone(s->context,0);}
static void start(doom_sound *s,uint8_t event,uint32_t now)
{
    unsigned n;const note *p=pattern(event,&n);(void)n;
    s->active=event;s->index=0;s->deadline=now+p[0].ms;s->tone(s->context,p[0].hz);
}
void doom_sound_update(doom_sound *s,uint32_t now)
{
    if(!s->active || (int32_t)(now-s->deadline)<0)return;
    unsigned n;const note *p=pattern(s->active,&n);
    if(++s->index<n){
        /* Raycasting can delay foreground service. Give each audible note
         * its full duration rather than dropping it or emitting a click. */
        s->deadline=now+p[s->index].ms;
        s->tone(s->context,p[s->index].hz);return;
    }
    s->active=0;
    uint8_t next=s->pending&DOOM_FIRE?DOOM_FIRE:s->pending&DOOM_KILL?DOOM_KILL:0;
    if(next){s->pending&=(uint8_t)~next;start(s,next,now);}
    else s->tone(s->context,0);
}
void doom_sound_events(doom_sound *s,uint8_t events,uint32_t now)
{
    doom_sound_update(s,now);
    if(events&DOOM_HURT){
        s->pending=(s->pending|(events&(DOOM_FIRE|DOOM_KILL)))&(DOOM_FIRE|DOOM_KILL);
        start(s,DOOM_HURT,now);return;
    }
    if(events&DOOM_KILL)s->pending|=DOOM_KILL;
    if(events&DOOM_FIRE){
        if(s->active==DOOM_HURT)s->pending|=DOOM_FIRE;
        else {s->pending&=(uint8_t)~DOOM_FIRE;start(s,DOOM_FIRE,now);}
    }else if(s->pending&DOOM_KILL){
        if(!s->active || s->active==DOOM_STEP){s->pending&=(uint8_t)~DOOM_KILL;start(s,DOOM_KILL,now);}
    }else if((events&DOOM_STEP) && !s->active)start(s,DOOM_STEP,now);
}
