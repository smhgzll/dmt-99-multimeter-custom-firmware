/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DOOM_CONTROLS_H
#define DOOM_CONTROLS_H
#include <stdbool.h>
#include <stdint.h>
/* Logical active-high keys; physical PC mapping lives in board.c. */
enum doom_button {DB_DOWN=1,DB_UP=2,DB_RIGHT=4,DB_LEFT=8,DB_FIRE=16,DB_POWER=32};
typedef struct {bool raw,stable,armed;uint32_t changed,next;} doom_key;
typedef struct {
    doom_key keys[6];uint32_t previous,chord_start;
    bool chord,chord_sent,chord_blocked;int8_t move_dir,turn_dir;
    int32_t move_speed,turn_speed,move_remainder,turn_remainder;
} doom_controls;
typedef struct {char key;bool power;int32_t move_q8,turn_q8;} doom_control_event;
void doom_controls_init(doom_controls *,uint8_t down,uint32_t now);
doom_control_event doom_controls_poll(doom_controls *,uint8_t down,uint32_t now,bool game);
#endif
