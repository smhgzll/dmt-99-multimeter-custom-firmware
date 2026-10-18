/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DMT_DOOM_RENDER_H
#define DMT_DOOM_RENDER_H
#include <stdint.h>
#include <stdbool.h>
typedef struct {
    void *context;
    void (*write)(void *,unsigned x,unsigned y,unsigned w,unsigned h,const uint16_t *pixels);
} doom_screen;
void doom_render(const doom_screen *);
void doom_weapon_menu(const doom_screen *,unsigned selected);
void doom_menu(const doom_screen *,unsigned selected,bool paused,bool god,bool sound);
void doom_fill(const doom_screen *,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color);
void doom_text(const doom_screen *,unsigned x,unsigned y,const char *,uint16_t fg,uint16_t bg,unsigned scale);
#endif
