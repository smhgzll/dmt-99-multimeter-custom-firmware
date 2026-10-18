/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DMT_DOOM_GAME_H
#define DMT_DOOM_GAME_H
#include <stdbool.h>
#include <stdint.h>
#define DOOM_RAYS 80
#define DOOM_VIEW_HEIGHT 200
#define DOOM_TICK_MS 100
#define DOOM_WEAPON_COUNT 6
#define DOOM_PROJECTILES 8
#define DOOM_MAX_SPRITES (188+DOOM_PROJECTILES)
enum doom_weapon {DOOM_FIST,DOOM_PISTOL,DOOM_SHOTGUN,DOOM_CHAINGUN,DOOM_SAW,DOOM_ROCKET};
typedef struct {float distance,fraction;uint16_t wall;} doom_ray;
typedef struct {float depth;int16_t center;uint8_t sprite,world_h,world_w,elevation;} doom_sprite;
typedef struct {
    float x,y;int16_t angle,health;uint16_t kills,pickups;
    bool complete;uint8_t shot,weapon,anim;
} doom_state;
typedef struct {const char *name;uint16_t damage,range;uint8_t cooldown,pellets,burst;} doom_weapon_info;
void doom_new_game(void);
void doom_set_service(void (*service)(void)); /* bounded IO pump; never changes game state */
bool doom_action(char key);
/* Q8 world units / degrees; swept collision, fractional turn precision. */
bool doom_motion(int32_t move_q8,int32_t turn_q8);
void doom_set_god(bool enabled);
bool doom_god_enabled(void);
bool doom_advance(uint32_t elapsed_ms);
uint32_t doom_get_tick(void);
void doom_set_weapon(unsigned weapon);
const doom_weapon_info *doom_weapon_details(unsigned weapon);
uint8_t doom_weapon_sprite(void);
int doom_weapon_bob(void);
enum doom_event {DOOM_STEP=1,DOOM_FIRE=2,DOOM_HURT=4,DOOM_KILL=8};
uint8_t doom_take_events(void);
doom_state doom_get_state(void);
void doom_cast(doom_ray rays[DOOM_RAYS]);
unsigned doom_project_sprites(doom_sprite *out,unsigned capacity);
bool doom_collision(float x,float y);
doom_ray doom_cast_at(float x,float y,float dx,float dy,bool filtered);
#endif
