/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DOOM_APP_H
#define DOOM_APP_H
#include "render.h"
#include <stdbool.h>
enum doom_app_mode {DOOM_MENU,DOOM_GAME,DOOM_HELP,DOOM_WEAPONS};
typedef struct {enum doom_app_mode mode;unsigned selected;bool has_game,dirty;unsigned weapon_choice;bool sound_enabled;} doom_app;
void doom_app_init(doom_app *);
void doom_app_input(doom_app *,char key);
void doom_app_update(doom_app *,uint32_t elapsed_ms);
void doom_app_motion(doom_app *,int32_t move_q8,int32_t turn_q8);
void doom_app_draw(doom_app *,const doom_screen *);
#endif
