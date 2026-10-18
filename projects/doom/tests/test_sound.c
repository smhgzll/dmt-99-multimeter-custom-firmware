/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "sound.h"
#include "game.h"
#include <assert.h>
#include <stdio.h>
static uint16_t heard[64];static unsigned used;
static void tone(void *c,uint16_t hz){(void)c;assert(used<64);heard[used++]=hz;}
int main(void)
{
    doom_sound s;doom_sound_init(&s,0,tone);assert(used==1 && heard[0]==0);
    doom_sound_events(&s,DOOM_STEP,100);assert(heard[1]==800);
    doom_sound_events(&s,DOOM_STEP,110);assert(used==2 && s.deadline==124);
    doom_sound_update(&s,123);assert(s.active);doom_sound_update(&s,124);assert(!s.active && heard[2]==0);
    used=0;doom_sound_events(&s,DOOM_FIRE|DOOM_KILL,200);
    assert(heard[0]==2600);doom_sound_update(&s,235);assert(heard[1]==1300);
    doom_sound_update(&s,270);assert(heard[2]==1200);doom_sound_update(&s,310);assert(heard[3]==1800);
    doom_sound_update(&s,350);assert(heard[4]==2400);doom_sound_update(&s,390);assert(heard[5]==0 && !s.active && !s.pending);
    used=0;doom_sound_events(&s,DOOM_FIRE,500);doom_sound_events(&s,DOOM_HURT,510);assert(heard[1]==330);
    doom_sound_events(&s,DOOM_FIRE|DOOM_KILL|DOOM_STEP,520);assert(s.pending==(DOOM_FIRE|DOOM_KILL));
    doom_sound_update(&s,610);assert(heard[2]==220);
    doom_sound_update(&s,710);assert(s.active==DOOM_FIRE && heard[3]==2600);
    doom_sound_update(&s,745);doom_sound_update(&s,780);assert(s.active==DOOM_KILL);
    doom_sound_stop(&s);assert(!s.active && !s.pending && heard[used-1]==0);
    used=0;doom_sound_events(&s,DOOM_STEP,0xfffffff0u);
    doom_sound_update(&s,7);assert(s.active);doom_sound_update(&s,8);assert(!s.active);
    used=0;doom_sound_events(&s,DOOM_FIRE,0);doom_sound_update(&s,1000);
    assert(used==2 && heard[0]==2600 && heard[1]==1300 && s.deadline==1035); /* delayed service preserves the next note */
    doom_sound_update(&s,1035);assert(!s.active && heard[2]==0);
    doom_sound_events(&s,0xf0,1100);assert(!s.active);
    puts("PASS: four tone patterns, shot/kill sequence, damage priority, bounded pending effects, cancellation, deadlines and wrap");
    return 0;
}
