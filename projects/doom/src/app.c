/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "app.h"
#include "game.h"
void doom_app_init(doom_app *a){*a=(doom_app){.mode=DOOM_MENU,.dirty=true,.sound_enabled=true};doom_set_god(false);doom_set_weapon(DOOM_PISTOL);doom_new_game();}
void doom_app_update(doom_app *a,uint32_t elapsed)
{if(a->mode==DOOM_GAME && doom_advance(elapsed))a->dirty=true;}
void doom_app_motion(doom_app *a,int32_t move_q8,int32_t turn_q8)
{if(a->mode==DOOM_GAME && doom_motion(move_q8,turn_q8))a->dirty=true;}
void doom_app_input(doom_app *a,char key)
{
    if(a->mode==DOOM_GAME){
        if(key=='p'){a->mode=DOOM_MENU;a->selected=0;a->dirty=true;}
        else if(doom_get_state().complete && key=='f'){doom_new_game();a->dirty=true;}
        else if(doom_action(key))a->dirty=true;
    }else if(a->mode==DOOM_WEAPONS){
        if(key=='w')a->weapon_choice=(a->weapon_choice+DOOM_WEAPON_COUNT-1)%DOOM_WEAPON_COUNT;
        else if(key=='s')a->weapon_choice=(a->weapon_choice+1)%DOOM_WEAPON_COUNT;
        else if(key=='f'){doom_set_weapon(a->weapon_choice);a->mode=DOOM_MENU;}
        else if(key=='p' || key=='a')a->mode=DOOM_MENU;
        a->dirty=true;
    }else if(a->mode==DOOM_HELP){
        if(key=='p' || key=='f' || key=='a'){a->mode=DOOM_MENU;a->dirty=true;}
    }else{
        unsigned count=a->has_game?6:5;
        if(key=='w'){a->selected=(a->selected+count-1)%count;a->dirty=true;}
        else if(key=='s'){a->selected=(a->selected+1)%count;a->dirty=true;}
        else if((key=='p' || key=='a') && a->has_game){a->mode=DOOM_GAME;a->dirty=true;}
        else if(key=='f'){
            if(a->selected==count-1)a->mode=DOOM_HELP;
            else if(a->selected==count-2)a->sound_enabled=!a->sound_enabled;
            else if(a->selected==count-3)doom_set_god(!doom_god_enabled());
            else if(a->selected==count-4){a->weapon_choice=doom_get_state().weapon;a->mode=DOOM_WEAPONS;}
            else {if(!a->has_game || a->selected==1)doom_new_game();a->has_game=true;a->mode=DOOM_GAME;}
            a->dirty=true;
        }
    }
}
void doom_app_draw(doom_app *a,const doom_screen *s)
{
    if(a->mode==DOOM_GAME)doom_render(s);
    else if(a->mode==DOOM_WEAPONS)doom_weapon_menu(s,a->weapon_choice);
    else if(a->mode==DOOM_MENU)doom_menu(s,a->selected,a->has_game,doom_god_enabled(),a->sound_enabled);
    else{
        doom_fill(s,0,0,320,240,0);doom_text(s,46,24,"POTAT-99 DOOM",0xf800,0,3);
        static const char *lines[]={"LEFT / RIGHT: TURN","UP / DOWN: MOVE","PC8: FIRE / OK","PC8 + DOWN 2S: MENU","HOLD: SPEED RAMPS UP","PC8: BACK"};
        for(unsigned i=0;i<6;++i)doom_text(s,24,72+i*24,lines[i],0xffff,0,2);
    }
    a->dirty=false;
}
