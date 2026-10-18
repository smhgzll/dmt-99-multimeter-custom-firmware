#!/usr/bin/env python3
"""Compare portable gameplay to unedited functions extracted from doom-avr.

Only AVR memory/UART syntax is shimmed. No AVR bootloader or flashing is run.
This is source-level port verification, not classic DOOM engine equivalence.
"""
import ctypes as c
import hashlib,json,math,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

class State(c.Structure):
    _fields_=[('x',c.c_float),('y',c.c_float),('angle',c.c_int16),('health',c.c_int16),
              ('kills',c.c_uint16),('pickups',c.c_uint16),('complete',c.c_bool),('shot',c.c_uint8)]
class Ray(c.Structure):
    _fields_=[('distance',c.c_float),('fraction',c.c_float),('wall',c.c_uint16)]

def main():
    original=ROOT/'upstream/doom-avr/avr/src/level1.c'
    raw=original.read_text()
    body=raw[raw.index('#define PLAYER_RADIUS'):raw.index('static void send_frame(void)')]
    shim='''#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "game.h"
#include "map_data.h"
#define memcpy_P memcpy
#define PSTR(s) s
static void uart_puts_P(const char *s){(void)s;}
static void request_chunk_P(const char *s){(void)s;}
'''
    wrappers='''
void oracle_new(void){move_step=32;rotate_step=15;memcpy(enemy_pos,enemies,sizeof enemy_pos);
memset(enemy_alive,255,sizeof enemy_alive);memset(item_collected,255,sizeof item_collected);
px=START_X;py=START_Y;angle_deg=START_ANGLE;player_health=100;kills=pickups=0;}
doom_state oracle_state(void){return (doom_state){px,py,angle_deg,player_health,kills,pickups,false,0};}
void oracle_action(char c){
if(c=='a')angle_deg=(angle_deg+rotate_step)%360;
else if(c=='d')angle_deg=(angle_deg-rotate_step+360)%360;
else if(c=='w'){try_move(move_step);check_bumps_and_pickups();check_goal();}
else if(c=='s'){try_move(-move_step);check_bumps_and_pickups();check_goal();}
else if(c=='f')do_fire();else return;
update_enemies();check_death();}
int oracle_collision(float x,float y){return collides(x,y);}
doom_ray oracle_ray(float x,float y,float dx,float dy){doom_ray r;
cast_ray(x,y,dx,dy,NULL,0,&r.distance,&r.wall,&r.fraction);return r;}
'''
    (ROOT/'build/upstream_oracle.c').write_text(shim+body+wrappers)
    subprocess.run(['gcc','-std=c11','-O2','-shared','-fPIC','-Isrc','-Igenerated',
                    'build/upstream_oracle.c','-lm','-o','build/upstream_oracle.so'],cwd=ROOT,check=True)
    old=c.CDLL(str(ROOT/'build/upstream_oracle.so'));new=c.CDLL(str(ROOT/'build/libdoom-host.so'))
    old.oracle_state.restype=new.doom_get_state.restype=State
    old.oracle_action.argtypes=new.doom_action.argtypes=[c.c_char]
    old.oracle_collision.argtypes=new.doom_collision.argtypes=[c.c_float,c.c_float]
    old.oracle_ray.argtypes=[c.c_float]*4;old.oracle_ray.restype=Ray
    new.doom_cast_at.argtypes=[c.c_float]*4+[c.c_bool];new.doom_cast_at.restype=Ray
    new.doom_collision.restype=c.c_bool
    rng=random.Random(99)
    counts={'collision_points':0,'ray_intersections':0}
    for _ in range(3000):
        x,y=rng.uniform(-800,3300),rng.uniform(-4500,1500)
        assert bool(old.oracle_collision(x,y))==new.doom_collision(x,y)
        counts['collision_points']+=1
        angle=rng.uniform(-math.pi,math.pi);dx,dy=math.cos(angle),math.sin(angle)
        a,b=old.oracle_ray(x,y,dx,dy),new.doom_cast_at(x,y,dx,dy,False)
        assert a.wall==b.wall and a.distance==b.distance and a.fraction==b.fraction
        counts['ray_intersections']+=1
    report={'pass':True,'upstream_commit':'f584566ec2e18041725e1de5668c6cb1f764ab58',
            'original_level1_sha256':hashlib.sha256(original.read_bytes()).hexdigest(),
            'cases':counts,'scope':'6000 preserved wall collision/raycast comparisons; old input-driven AI/combat comparisons retired because 0.3 uses timed AI and new weapons'}
    (ROOT/'build/upstream_differential.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':main()
