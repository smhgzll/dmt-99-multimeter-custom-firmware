/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DOOM_SOUND_H
#define DOOM_SOUND_H
#include <stdint.h>
typedef struct {
    void *context;
    void (*tone)(void *,uint16_t hz); /* zero = silence */
    uint32_t deadline;
    uint8_t active,index,pending;
} doom_sound;
void doom_sound_init(doom_sound *,void *,void (*tone)(void *,uint16_t));
void doom_sound_events(doom_sound *,uint8_t events,uint32_t now);
void doom_sound_update(doom_sound *,uint32_t now);
void doom_sound_stop(doom_sound *);
#endif
