/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Private fixtures, no test setter linked into firmware.
 */
#include "../src/game.c"
#include <assert.h>
#include <stdio.h>
static void isolated(void){doom_set_god(false);doom_set_weapon(DOOM_PISTOL);doom_new_game();memset(enemy_alive,0,sizeof enemy_alive);}
static void actor(unsigned i,int x,int y,int hp,uint8_t state)
{enemy_alive[i>>3]|=(uint8_t)(1u<<(i&7));enemy_pos[i]=(point_t){x,y};enemy_health[i]=(int16_t)hp;enemy_state[i]=state;enemy_timer[i]=enemy_cooldown[i]=0;}
static void steps(unsigned n){for(unsigned i=0;i<n;++i)doom_advance(100);}
int main(void)
{
    isolated();assert(!doom_take_events());doom_action('w');assert(doom_take_events()==DOOM_STEP);
    doom_action('a');assert(!doom_take_events());doom_action('f');assert(doom_take_events()==DOOM_FIRE);
    isolated();px=1072;py=224;doom_action('w');assert(px==1072 && !doom_take_events());
    isolated();actor(0,160,-272,35,CHASE);point_t before=enemy_pos[0];
    doom_action('f');assert(kills==0 && enemy_health[0]==15 && enemy_pos[0].x==before.x && enemy_pos[0].y==before.y);
    assert(doom_take_events()==DOOM_FIRE);steps(4);doom_take_events();doom_action('f');
    assert(kills==1 && doom_take_events()==(DOOM_FIRE|DOOM_KILL));
    assert(enemy_state[0]==DYING);steps(6);assert(enemy_state[0]==DEAD);
    isolated();actor(0,140,-272,35,CHASE);doom_action('w');doom_action('w');doom_action('w');
    assert(kills==0 && bit_get(enemy_alive,0) && px<=112); /* no bump insta-kill */
    assert(doom_take_events()==DOOM_STEP);
    unsigned human=0;while(human<ENEMY_COUNT && doom_enemy_class[human]!=0)++human;assert(human<ENEMY_COUNT);
    isolated();actor(human,160,-272,35,CHASE);player_health=6;steps(4);
    assert(player_health==100 && (doom_take_events()&DOOM_HURT)); /* timed lethal attack survives respawn */
    isolated();actor(human,500,-272,35,IDLE);angle_deg=90;doom_action('f');assert(enemy_state[human]==CHASE);
    point_t p=enemy_pos[human];doom_action('a');doom_action('d');assert(enemy_pos[human].x==p.x && enemy_pos[human].y==p.y);
    steps(1);assert(enemy_pos[human].x<p.x); /* moves without another key */
    isolated();px=1064;py=224;actor(human,1112,224,35,IDLE);steps(12);
    assert(enemy_state[human]==IDLE && player_health==100); /* wall blocks sight */
    doom_action('f');assert(enemy_state[human]==IDLE); /* no hearing through walls */
    enemy_state[human]=CHASE;enemy_memory[human]=50;enemy_target[human]=(point_t){1064,224};
    point_t remembered=enemy_target[human];px=1040;py=210;steps(1);
    assert(enemy_target[human].x==remembered.x && enemy_target[human].y==remembered.y);
    steps(80);assert(!circle_collides(enemy_pos[human].x,enemy_pos[human].y,ENEMY_RADIUS));
    assert(!line_clear(1064,224,1112,224)); /* the barrier remains, no wall penetration */
    isolated();px=1064;py=224;actor(human,1112,224,35,WINDUP);
    enemy_memory[human]=50;enemy_target[human]=(point_t){1064,224};enemy_timer[human]=1;
    steps(1);assert(player_health==100 && !(doom_take_events()&DOOM_HURT));
    assert(spawn_projectile(1112,224,-16,0,1));steps(8);assert(player_health==100 && !missiles[0].life);
    explode(1112,224);assert(player_health==100); /* wall also blocks splash */
    isolated();px=1064;py=224;actor(human,1112,224,35,CHASE);
    enemy_memory[human]=5;enemy_target[human]=enemy_pos[human];steps(5);
    assert(enemy_state[human]==IDLE && player_health==100); /* forgets hidden player */
    isolated();actor(human,160,-272,35,CHASE);doom_set_weapon(DOOM_SHOTGUN);doom_action('f');assert(kills==1);
    isolated();actor(human,500,-272,100,IDLE);doom_set_weapon(DOOM_ROCKET);doom_action('f');
    assert(missiles[0].life && enemy_pos[human].x==500);steps(12);assert(kills>=1);
    isolated();doom_set_weapon(DOOM_CHAINGUN);doom_action('f');assert(burst_left==2);doom_take_events();steps(2);assert(!burst_left && (doom_take_events()&DOOM_FIRE));
    const unsigned expected[]={24,20,60,36,36,100};
    for(unsigned w=0;w<DOOM_WEAPON_COUNT;++w){
        isolated();actor(human,136,-272,200,IDLE);doom_set_weapon(w);doom_action('f');steps(2);
        assert(enemy_health[human]==200-(int)expected[w]);
    }
    isolated();doom_set_weapon(DOOM_SHOTGUN);doom_action('f');uint8_t firing=doom_weapon_sprite();
    steps(3);assert(doom_weapon_sprite()!=firing);steps(5);assert(!weapon_cooldown && !shot);
    isolated();px=1064;py=224;actor(human,1112,224,100,CHASE);doom_action('f');assert(enemy_health[human]==100);
    unsigned watcher=0;while(watcher<ENEMY_COUNT && doom_enemy_facing[watcher]!=4)++watcher;assert(watcher<ENEMY_COUNT);
    isolated();actor(watcher,256,-272,100,IDLE);steps(4);assert(enemy_state[watcher]!=IDLE);
    isolated();doom_sprite sprites[DOOM_MAX_SPRITES];unsigned n=doom_project_sprites(sprites,DOOM_MAX_SPRITES);
    for(unsigned i=0;i<n;++i)assert(sprites[i].world_h<=24); /* pickups are small */
    isolated();actor(human,160,-272,35,CHASE);doom_set_god(true);steps(40);
    assert(player_health==100 && !(doom_take_events()&DOOM_HURT));
    assert(spawn_projectile(px-20,py,16,0,1));steps(2);assert(player_health==100);
    doom_set_god(false);steps(20);assert(player_health<100);
    isolated();actor(human,160,-272,1,IDLE);doom_action('f');steps(6);
    assert(enemy_state[human]==DEAD);steps(79);assert(enemy_state[human]==DEAD);
    steps(1);assert(enemy_state[human]==REMOVED && kills==1 && !bit_get(enemy_alive,human));
    memset(item_collected,0,sizeof item_collected);assert(!doom_project_sprites(sprites,DOOM_MAX_SPRITES));
    isolated();assert(doom_motion(128,128));assert(px>96 && px<97 && angle_deg==0 && angle_fraction==128);
    assert(doom_motion(0,-256) && angle_deg==359 && angle_fraction==128);
    unsigned barriers=0;
    for(unsigned i=0;i<WALL_COUNT;++i){
        const wall_t *w=walls+i;float dx=w->x2-w->x1,dy=w->y2-w->y1,length=sqrtf(dx*dx+dy*dy);
        if(length<8)continue;
        float x=(w->x1+w->x2)/2.0f,y=(w->y1+w->y2)/2.0f;
        assert(!line_clear(x-dy*32/length,y+dx*32/length,x+dy*32/length,y-dx*32/length));++barriers;
    }
    assert(!doom_action('q'));
    printf("PASS: AI, last-seen pursuit, wall-gated hearing, %u sight barriers, collision/attacks, god, corpse expiry, fractional movement, six weapons/projectiles\n",barriers);
    return 0;
}
