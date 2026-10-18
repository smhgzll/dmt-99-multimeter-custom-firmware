/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Ray intersection and circle/wall collision derive from doom-avr (Talha Berk
 * Arslan, 2026). Timed AI, weapons, projectiles and actor states are local.
 */
#include <math.h>
#include <stddef.h>
#include <string.h>
#include "game.h"
#include "map_data.h"
#include "assets.h"
#define PLAYER_RADIUS 16.0f
#define PICKUP_RADIUS 28.0f
#define PLAYER_MAX_HEALTH 100
#define ENEMY_RADIUS 12.0f

enum actor_state {IDLE,CHASE,WINDUP,ATTACK,PAIN,DYING,DEAD,REMOVED};
static float px,py;
static int16_t angle_deg,player_health;
static uint8_t angle_fraction;
static bool god_mode;
static uint16_t kills,pickups;
static bool level_complete;
static uint8_t shot,events,weapon=DOOM_PISTOL,weapon_cooldown,burst_left,walk_bob;
static uint16_t accumulator;
uint32_t doom_simulation_tick;
static void (*service_io)(void);
void doom_set_service(void (*service)(void)){service_io=service;}
static void pump(void){if(service_io)service_io();}
static point_t enemy_pos[ENEMY_COUNT];
/* Last observed position, never the live position through an occluder. */
static point_t enemy_target[ENEMY_COUNT];
static uint8_t enemy_memory[ENEMY_COUNT];
static uint8_t enemy_alive[(ENEMY_COUNT+7)/8],item_collected[(ITEM_COUNT+7)/8];
static uint8_t enemy_state[ENEMY_COUNT],enemy_timer[ENEMY_COUNT],enemy_cooldown[ENEMY_COUNT];
static int16_t enemy_health[ENEMY_COUNT];
static uint8_t detour_ticks[ENEMY_COUNT];
static int8_t detour_x[ENEMY_COUNT],detour_y[ENEMY_COUNT];
typedef struct {float x,y,vx,vy;uint8_t kind,life;} projectile;
static projectile missiles[DOOM_PROJECTILES];
static const doom_weapon_info weapons[DOOM_WEAPON_COUNT]={
 {"FIST",24,64,5,1,1},{"PISTOL",20,2200,4,1,1},{"SHOTGUN",12,1500,8,5,1},
 {"CHAINGUN",12,2200,4,1,3},{"CHAINSAW",12,72,4,1,3},{"ROCKET",100,2400,9,1,1}};
static uint8_t bit_get(const uint8_t *a,unsigned i){return (a[i>>3]>>(i&7))&1;}
static void bit_clear(uint8_t *a,unsigned i){a[i>>3]&=(uint8_t)~(1u<<(i&7));}
static float deg2rad(float d){return d*3.14159265f/180.0f;}
static float view_angle(void){return deg2rad(angle_deg+angle_fraction/256.0f);}
static uint8_t circle_collides(float x, float y,float radius)
{
    for (uint16_t i = 0; i < WALL_COUNT; i++) {
        wall_t w;
        memcpy(&w, &walls[i], sizeof(wall_t));

        int16_t minx = (w.x1 < w.x2) ? w.x1 : w.x2;
        int16_t maxx = (w.x1 > w.x2) ? w.x1 : w.x2;
        int16_t miny = (w.y1 < w.y2) ? w.y1 : w.y2;
        int16_t maxy = (w.y1 > w.y2) ? w.y1 : w.y2;
        if (x < minx - radius || x > maxx + radius ||
            y < miny - radius || y > maxy + radius)
            continue;

        float dx = w.x2 - w.x1, dy = w.y2 - w.y1;
        float len2 = dx * dx + dy * dy;
        float t = 0.0f;
        if (len2 > 0.0f) {
            t = ((x - w.x1) * dx + (y - w.y1) * dy) / len2;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
        }
        float projx = w.x1 + t * dx;
        float projy = w.y1 + t * dy;
        float ddx = x - projx, ddy = y - projy;
        if (ddx * ddx + ddy * ddy < radius * radius)
            return 1;
    }
    return 0;
}


static uint8_t collides(float x,float y){return circle_collides(x,y,PLAYER_RADIUS);}
static void cast_ray(float ox, float oy, float dx, float dy,
                      const uint16_t *candidates, uint16_t num_candidates,
                      float *out_t, uint16_t *out_wall, float *out_s)
{
    float best_t = -1.0f, best_s = 0.0f;
    uint16_t best_i = 0xFFFF;
    uint16_t count = candidates ? num_candidates : WALL_COUNT;
    for (uint16_t c = 0; c < count; c++) {
        uint16_t i = candidates ? candidates[c] : c;
        wall_t w;
        memcpy(&w, &walls[i], sizeof(wall_t));
        float bx = w.x2 - w.x1, by = w.y2 - w.y1;
        float denom = dx * by - dy * bx;
        if (denom > -1e-6f && denom < 1e-6f) continue;
        float t = ((w.x1 - ox) * by - (w.y1 - oy) * bx) / denom;
        if (t <= 0.0f) continue;
        float s = ((w.x1 - ox) * dy - (w.y1 - oy) * dx) / denom;
        if (s < 0.0f || s > 1.0f) continue;
        if (best_i == 0xFFFF || t < best_t) { best_t = t; best_i = i; best_s = s; }
    }
    *out_t = best_t; *out_wall = best_i; *out_s = best_s;
}

/* Division-free segment visibility. Integer map coordinates; conservative
 * endpoint contacts stop shots and vision, including collinear overlap. */
static int64_t cross(int32_t ax,int32_t ay,int32_t bx,int32_t by)
{return (int64_t)ax*by-(int64_t)ay*bx;}
static bool opposite(int64_t a,int64_t b){return (a<=0 && b>=0)||(a>=0 && b<=0);}
static bool line_clear(float ax,float ay,float bx,float by)
{
    int32_t x0=(int32_t)ax,y0=(int32_t)ay,x1=(int32_t)bx,y1=(int32_t)by;
    for(unsigned i=0;i<WALL_COUNT;++i){
        const wall_t *w=walls+i;
        if((x0<w->x1 && x0<w->x2 && x1<w->x1 && x1<w->x2) ||
           (x0>w->x1 && x0>w->x2 && x1>w->x1 && x1>w->x2) ||
           (y0<w->y1 && y0<w->y2 && y1<w->y1 && y1<w->y2) ||
           (y0>w->y1 && y0>w->y2 && y1>w->y1 && y1>w->y2))continue;
        int64_t a=cross(x1-x0,y1-y0,w->x1-x0,w->y1-y0),b=cross(x1-x0,y1-y0,w->x2-x0,w->y2-y0);
        int64_t c=cross(w->x2-w->x1,w->y2-w->y1,x0-w->x1,y0-w->y1),d=cross(w->x2-w->x1,w->y2-w->y1,x1-w->x1,y1-w->y1);
        if(opposite(a,b) && opposite(c,d))return false;
    }
    return true;
}
static bool player_blocked(float x,float y)
{
    if(collides(x,y))return true;
    for(unsigned i=0;i<ENEMY_COUNT;++i)if(bit_get(enemy_alive,i)){
        float dx=x-enemy_pos[i].x,dy=y-enemy_pos[i].y;
        if(dx*dx+dy*dy<28*28)return true;
    }
    return false;
}
static void try_move(float distance)
{
    unsigned steps=(unsigned)(fabsf(distance)/8)+1;
    float a=view_angle(),dx=cosf(a)*distance/steps,dy=sinf(a)*distance/steps,ox=px,oy=py;
    for(unsigned n=0;n<steps;++n){
        if(!player_blocked(px+dx,py+dy)){px+=dx;py+=dy;}
        else if(!player_blocked(px+dx,py))px+=dx;
        else if(!player_blocked(px,py+dy))py+=dy;
    }
    if(px!=ox || py!=oy){events|=DOOM_STEP;walk_bob=6;}
}
static void pickups_and_goal(void)
{
    for(unsigned i=0;i<ITEM_COUNT;++i)if(bit_get(item_collected,i)){
        float dx=px-items[i].x,dy=py-items[i].y;
        if(dx*dx+dy*dy<PICKUP_RADIUS*PICKUP_RADIUS && line_clear(px,py,items[i].x,items[i].y)){
            bit_clear(item_collected,i);++pickups;
            player_health+=(int16_t)doom_item_heal[i];if(player_health>100)player_health=100;
        }
    }
    float dx=px-GOAL_X,dy=py-GOAL_Y;
    if(dx*dx+dy*dy<GOAL_RADIUS*GOAL_RADIUS)level_complete=true;
}
static void hurt_player(unsigned damage)
{
    if(god_mode)return;
    events|=DOOM_HURT;player_health-=(int16_t)damage;
    if(player_health<=0){px=START_X;py=START_Y;angle_deg=START_ANGLE;angle_fraction=0;player_health=100;shot=0;burst_left=0;}
}
static void hurt_enemy(unsigned i,unsigned damage)
{
    if(!bit_get(enemy_alive,i))return;
    enemy_health[i]-=(int16_t)damage;
    if(enemy_health[i]<=0){bit_clear(enemy_alive,i);enemy_state[i]=DYING;enemy_timer[i]=6;++kills;events|=DOOM_KILL;}
    else {enemy_state[i]=PAIN;enemy_timer[i]=2;enemy_target[i]=(point_t){(int16_t)px,(int16_t)py};enemy_memory[i]=50;}
}
static bool enemy_blocked(unsigned self,int x,int y)
{
    if(circle_collides((float)x,(float)y,ENEMY_RADIUS))return true;
    float pdx=x-px,pdy=y-py;if(pdx*pdx+pdy*pdy<28*28)return true;
    for(unsigned i=0;i<ENEMY_COUNT;++i)if(i!=self && bit_get(enemy_alive,i)){
        int dx=x-enemy_pos[i].x,dy=y-enemy_pos[i].y;
        if(dx*dx+dy*dy<24*24)return true;
    }
    return false;
}
static void move_enemy(unsigned i,float dx,float dy)
{
    float length=sqrtf(dx*dx+dy*dy);if(length<1)return;
    float step=doom_enemy_class[i]==3?8.0f:5.0f;
    int mx=(int)(dx*step/length),my=(int)(dy*step/length),x=enemy_pos[i].x,y=enemy_pos[i].y;
    if(detour_ticks[i]){
        if(line_clear(x,y,enemy_target[i].x,enemy_target[i].y) && !enemy_blocked(i,x+mx,y+my))detour_ticks[i]=0;
        else if(!enemy_blocked(i,x+detour_x[i],y+detour_y[i])){
            enemy_pos[i].x+=detour_x[i];enemy_pos[i].y+=detour_y[i];--detour_ticks[i];return;
        }else detour_ticks[i]=0;
    }
    if(!enemy_blocked(i,x+mx,y+my)){enemy_pos[i].x+=(int16_t)mx;enemy_pos[i].y+=(int16_t)my;return;}
    /* Keep a detour direction while a barrier hides the target; do not
     * oscillate back toward it on every tick. Global path search is absent. */
    int sign=(i&1)?1:-1;
    for(unsigned n=0;n<2;++n){int sx=-my*sign,sy=mx*sign;
        if((sx || sy) && !enemy_blocked(i,x+sx,y+sy)){
            detour_x[i]=(int8_t)sx;detour_y[i]=(int8_t)sy;detour_ticks[i]=48;
            enemy_pos[i].x+=(int16_t)sx;enemy_pos[i].y+=(int16_t)sy;return;
        }sign=-sign;
    }
    if(mx && !enemy_blocked(i,x+mx,y))enemy_pos[i].x+=(int16_t)mx;
    else if(my && !enemy_blocked(i,x,y+my))enemy_pos[i].y+=(int16_t)my;

}
static bool spawn_projectile(float x,float y,float vx,float vy,uint8_t kind)
{
    for(unsigned i=0;i<DOOM_PROJECTILES;++i)if(!missiles[i].life){missiles[i]=(projectile){x,y,vx,vy,kind,80};return true;}
    return false;
}
static void explode(float x,float y)
{
    for(unsigned i=0;i<ENEMY_COUNT;++i)if(bit_get(enemy_alive,i)){
        float dx=x-enemy_pos[i].x,dy=y-enemy_pos[i].y,d=dx*dx+dy*dy;
        if(d<96*96 && line_clear(x,y,enemy_pos[i].x,enemy_pos[i].y))hurt_enemy(i,d<48*48?100:50);
    }
    float dx=x-px,dy=y-py;
    if(dx*dx+dy*dy<64*64 && line_clear(x,y,px,py))hurt_player(30);
}
static void update_projectiles(void)
{
    for(unsigned i=0;i<DOOM_PROJECTILES;++i){projectile *p=missiles+i;if(!p->life)continue;
        --p->life;
        for(unsigned sub=0;sub<4 && p->life;++sub){
            float nx=p->x+p->vx/4,ny=p->y+p->vy/4;bool hit=!line_clear(p->x,p->y,nx,ny);
            if(!hit){p->x=nx;p->y=ny;
                if(p->kind==0){
                    for(unsigned j=0;j<ENEMY_COUNT;++j)if(bit_get(enemy_alive,j)){
                        float dx=nx-enemy_pos[j].x,dy=ny-enemy_pos[j].y;
                        if(dx*dx+dy*dy<18*18){hit=true;break;}
                    }
                }else {float dx=nx-px,dy=ny-py;if(dx*dx+dy*dy<18*18){hurt_player(12);hit=true;}}
            }
            if(hit){p->life=0;if(p->kind==0)explode(p->x,p->y);}
        }
    }
}
static void hitscan(float dx,float dy,unsigned damage,unsigned range)
{
    float wt,ws;uint16_t wi;cast_ray(px,py,dx,dy,NULL,0,&wt,&wi,&ws);
    float best=(float)range;int target=-1;
    if(wi!=0xffff && wt<best)best=wt;
    for(unsigned i=0;i<ENEMY_COUNT;++i)if(bit_get(enemy_alive,i)){
        float ex=enemy_pos[i].x-px,ey=enemy_pos[i].y-py,fwd=ex*dx+ey*dy,side= -ex*dy+ey*dx;
        if(fwd>0 && fwd<best && side*side<18*18){best=fwd;target=(int)i;}
    }
    if(target>=0)hurt_enemy((unsigned)target,damage);
}
static void fire_weapon(void)
{
    const doom_weapon_info *w=weapons+weapon;
    float a=view_angle(),dx=cosf(a),dy=sinf(a);
    shot=2;events|=DOOM_FIRE;
    for(unsigned i=0;i<ENEMY_COUNT;++i)if(enemy_state[i]==IDLE){
        float x=enemy_pos[i].x-px,y=enemy_pos[i].y-py;
        if(x*x+y*y<1000*1000 && line_clear(px,py,enemy_pos[i].x,enemy_pos[i].y)){
            enemy_state[i]=CHASE;enemy_target[i]=(point_t){(int16_t)px,(int16_t)py};enemy_memory[i]=50;
        }
    }
    if(weapon==DOOM_ROCKET)spawn_projectile(px,py,dx*32,dy*32,0);
    else for(unsigned p=0;p<w->pellets;++p){
        float spread=w->pellets==1?0:((int)p-2)*0.035f;
        float rx=dx-dy*spread,ry=dy+dx*spread,inv=1.0f/sqrtf(rx*rx+ry*ry);
        hitscan(rx*inv,ry*inv,w->damage,w->range);
    }
}
static void update_enemies(void)
{
    static const int8_t facing_x[]={127,90,0,-90,-127,-90,0,90},facing_y[]={0,90,127,90,0,-90,-127,-90};
    for(unsigned i=0;i<ENEMY_COUNT;++i){
        if(!(i&3))pump();
        if(enemy_cooldown[i])--enemy_cooldown[i];
        if(enemy_timer[i])--enemy_timer[i];
        if(enemy_state[i]==DYING){if(!enemy_timer[i]){enemy_state[i]=DEAD;enemy_timer[i]=80;}continue;}
        if(enemy_state[i]==DEAD){if(!enemy_timer[i])enemy_state[i]=REMOVED;continue;}
        if(!bit_get(enemy_alive,i))continue;
        float dx=px-enemy_pos[i].x,dy=py-enemy_pos[i].y,d2=dx*dx+dy*dy;
        if(enemy_state[i]==IDLE){
            if((doom_simulation_tick+i)%4 || d2>900*900)continue;
            unsigned f=doom_enemy_facing[i];
            if(dx*facing_x[f]+dy*facing_y[f]<=0 || !line_clear(enemy_pos[i].x,enemy_pos[i].y,px,py))continue;
            enemy_state[i]=CHASE;
        }
        if(enemy_state[i]==PAIN){if(enemy_timer[i])continue;enemy_state[i]=CHASE;}
        if(enemy_state[i]==ATTACK){if(enemy_timer[i])continue;enemy_state[i]=CHASE;}
        unsigned kind=doom_enemy_class[i];float range=kind==3?48.0f:kind==2?400.0f:320.0f;
        bool visible=d2<1000*1000 && line_clear(enemy_pos[i].x,enemy_pos[i].y,px,py);
        if(visible){enemy_target[i]=(point_t){(int16_t)px,(int16_t)py};enemy_memory[i]=50;}
        else if(!enemy_memory[i] || !--enemy_memory[i]){enemy_state[i]=IDLE;detour_ticks[i]=0;continue;}
        if(enemy_state[i]==WINDUP){
            if(enemy_timer[i])continue;
            if(visible && d2<=range*range){
                if(kind==2){float inv=16.0f/sqrtf(d2<1?1:d2);spawn_projectile(enemy_pos[i].x,enemy_pos[i].y,dx*inv,dy*inv,1);}
                else hurt_player(kind==3?12:kind==1?10:6);
            }
            enemy_state[i]=ATTACK;enemy_timer[i]=2;enemy_cooldown[i]=kind==2?12:8;continue;
        }
        if(visible && d2<=range*range && !enemy_cooldown[i]){enemy_state[i]=WINDUP;enemy_timer[i]=3;}
        else {
            float tx=enemy_target[i].x-enemy_pos[i].x,ty=enemy_target[i].y-enemy_pos[i].y;
            if(tx*tx+ty*ty>28*28)move_enemy(i,tx,ty);
        }
    }
}
static void tick(void)
{
    ++doom_simulation_tick;
    if(shot)--shot;
    if(walk_bob)--walk_bob;
    if(weapon_cooldown)--weapon_cooldown;
    if(burst_left){fire_weapon();--burst_left;}
    update_projectiles();update_enemies();
}
void doom_new_game(void)
{
    memcpy(enemy_pos,enemies,sizeof enemy_pos);memset(enemy_alive,255,sizeof enemy_alive);memset(item_collected,255,sizeof item_collected);
    memset(enemy_memory,0,sizeof enemy_memory);memcpy(enemy_target,enemies,sizeof enemy_target);
    memset(detour_ticks,0,sizeof detour_ticks);memset(enemy_state,IDLE,sizeof enemy_state);memset(enemy_timer,0,sizeof enemy_timer);memset(enemy_cooldown,0,sizeof enemy_cooldown);memset(missiles,0,sizeof missiles);
    for(unsigned i=0;i<ENEMY_COUNT;++i)enemy_health[i]=doom_enemy_class[i]==3?120:doom_enemy_class[i]==2?60:35;
    px=START_X;py=START_Y;angle_deg=START_ANGLE;angle_fraction=0;player_health=100;kills=pickups=0;level_complete=false;
    shot=events=weapon_cooldown=burst_left=walk_bob=0;doom_simulation_tick=0;accumulator=0;
}
bool doom_action(char c)
{
    if(level_complete)return false;
    switch(c){
    case 'a':return doom_motion(0,3*256);
    case 'd':return doom_motion(0,-3*256);
    case 'w':return doom_motion(8*256,0);
    case 's':return doom_motion(-8*256,0);
    case 'f':if(weapon_cooldown)return false;fire_weapon();weapon_cooldown=weapons[weapon].cooldown;burst_left=weapons[weapon].burst-1;break;
    default:return false;
    }
    return true;
}
bool doom_motion(int32_t move_q8,int32_t turn_q8)
{
    if(level_complete || (!move_q8 && !turn_q8))return false;
    /* Bounded queued motion: at most 64 world units / 45 degrees per drain. */
    if(move_q8>64*256)move_q8=64*256;
    if(move_q8< -64*256)move_q8=-64*256;
    if(turn_q8>45*256)turn_q8=45*256;
    if(turn_q8< -45*256)turn_q8=-45*256;
    int32_t angle=(angle_deg*256+angle_fraction+turn_q8+360*256)%(360*256);
    angle_deg=(int16_t)(angle/256);angle_fraction=(uint8_t)(angle%256);
    if(move_q8){try_move(move_q8/256.0f);pickups_and_goal();}
    return true;
}
void doom_set_god(bool enabled){god_mode=enabled;}
bool doom_god_enabled(void){return god_mode;}
bool doom_advance(uint32_t ms)
{
    if(level_complete)return false;
    if(ms>500)ms=500;
    accumulator=(uint16_t)(accumulator+ms);bool changed=false;
    while(accumulator>=DOOM_TICK_MS){accumulator-=DOOM_TICK_MS;tick();changed=true;}
    return changed;
}
uint32_t doom_get_tick(void){return doom_simulation_tick;}
uint8_t doom_take_events(void){uint8_t v=events;events=0;return v;}
doom_state doom_get_state(void){return (doom_state){px,py,angle_deg,player_health,kills,pickups,level_complete,shot,weapon,(uint8_t)doom_simulation_tick};}
bool doom_collision(float x,float y){return collides(x,y)!=0;}
void doom_set_weapon(unsigned w){if(w<DOOM_WEAPON_COUNT){weapon=(uint8_t)w;weapon_cooldown=burst_left=shot=0;}}
const doom_weapon_info *doom_weapon_details(unsigned w){return weapons+(w<DOOM_WEAPON_COUNT?w:DOOM_PISTOL);}
uint8_t doom_weapon_sprite(void)
{
    unsigned frame=0,total=weapons[weapon].cooldown;
    if(weapon_cooldown){unsigned elapsed=total-weapon_cooldown;frame=1+(elapsed*3/total);if(frame>3)frame=3;}
    return doom_weapon_frames[weapon*4+frame];
}
int doom_weapon_bob(void){static const int8_t wave[]={0,2,3,2,0,-2,-3,-2};return walk_bob?wave[doom_simulation_tick&7]:0;}
/* Full-capacity list: unlike the AVR's 140-entry cap, no candidate is lost.
 * It costs 808 bytes of static RAM and avoids view-dependent missing walls. */
static uint16_t candidates[WALL_COUNT];
static unsigned candidate_count;
static void select_candidates(void)
{
    float a=view_angle(), ca=cosf(a),sa=sinf(a);
    candidate_count=0;
    for(unsigned i=0;i<WALL_COUNT;++i){
        const wall_t *w=&walls[i];
        float f1=(w->x1-px)*ca+(w->y1-py)*sa;
        float f2=(w->x2-px)*ca+(w->y2-py)*sa;
        if(f1<=0 && f2<=0)continue;
        if(f1>0 && f2>0){
            float s1=-(w->x1-px)*sa+(w->y1-py)*ca;
            float s2=-(w->x2-px)*sa+(w->y2-py)*ca;
            /* tan(48 degrees), conservative for the 60-degree view. */
            if((s1>f1*1.110613f && s2>f2*1.110613f) ||
               (s1< -f1*1.110613f && s2< -f2*1.110613f))continue;
        }
        candidates[candidate_count++]=(uint16_t)i;
    }
}
doom_ray doom_cast_at(float x,float y,float dx,float dy,bool filtered)
{
    doom_ray r;
    cast_ray(x,y,dx,dy,filtered?candidates:NULL,(uint16_t)candidate_count,&r.distance,&r.wall,&r.fraction);
    return r;
}
void doom_cast(doom_ray rays[DOOM_RAYS])
{
    select_candidates();
    float a=view_angle(),ca=cosf(a),sa=sinf(a);
    for(unsigned col=0;col<DOOM_RAYS;++col){
        if(!(col&3))pump();
        /* Camera-plane rays have depth corrected by construction. Positive
         * world Y is north: the left edge looks +30 degrees, not mirrored. */
        float side=(1.0f-2.0f*(float)col/(DOOM_RAYS-1))*0.577350269f;
        rays[col]=doom_cast_at(px,py,ca-sa*side,sa+ca*side,true);
        if(rays[col].distance<1)rays[col].distance=1;
    }
}
unsigned doom_project_sprites(doom_sprite *out,unsigned cap)
{
    float a=view_angle(),ca=cosf(a),sa=sinf(a);unsigned n=0;
    for(unsigned i=0;i<ENEMY_COUNT+ITEM_COUNT+DOOM_PROJECTILES && n<cap;++i){
        if(!(i&15))pump();
        float x,y;uint8_t sprite,h,z=0;
        if(i<ENEMY_COUNT){
            unsigned frame=0;uint8_t st=enemy_state[i];
            if(!bit_get(enemy_alive,i) && st!=DYING && st!=DEAD)continue;
            if(st==CHASE)frame=(doom_simulation_tick/2+i)%4;
            else if(st==WINDUP)frame=4;
            else if(st==ATTACK)frame=5;
            else if(st==PAIN)frame=6;
            else if(st==DYING)frame=7+(6-enemy_timer[i])/2;
            else if(st==DEAD)frame=9;
            sprite=doom_enemy_frames[doom_enemy_class[i]*10+frame];
            h=st==DEAD?16:st==DYING?36:56;x=enemy_pos[i].x;y=enemy_pos[i].y;
        }else if(i<ENEMY_COUNT+ITEM_COUNT){
            unsigned j=i-ENEMY_COUNT;if(!bit_get(item_collected,j))continue;
            sprite=doom_item_sprite[j];h=doom_item_height[j];x=items[j].x;y=items[j].y;
        }else {projectile *p=missiles+(i-ENEMY_COUNT-ITEM_COUNT);if(!p->life)continue;
            x=p->x;y=p->y;z=24;h=p->kind?12:10;sprite=doom_projectile_frames[p->kind?1+(doom_simulation_tick&1):0];}
        float dx=x-px,dy=y-py,depth=dx*ca+dy*sa;if(depth<=8)continue;
        float ratio=(dx*sa-dy*ca)/depth;if(ratio< -1.5f || ratio>1.5f)continue;
        unsigned w=sprite<SPRITE_COUNT?(unsigned)h*doom_sprite_width[sprite]/doom_sprite_height[sprite]:h;
        if(w>96)w=96;
        if(w<2)w=2;
        doom_sprite s={depth,(int16_t)(160+ratio*277.128129f),sprite,h,(uint8_t)w,z};
        unsigned k=n;while(k && out[k-1].depth<s.depth){out[k]=out[k-1];--k;}out[k]=s;++n;
    }
    return n;
}
