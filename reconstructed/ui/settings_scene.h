#ifndef DMT_SETTINGS_SCENE_H
#define DMT_SETTINGS_SCENE_H
#include "view.h"
void settings_scene_theme(const dmt_view *,uint8_t display_layout,uint16_t settings);
void settings_scene_measurement(const dmt_view *,uint8_t display_layout,uint16_t *settings);
void settings_scene_auto_off(const dmt_view *,uint8_t display_layout,uint16_t settings);
void settings_brightness_image(const dmt_view *,int selected,int page,unsigned level,uint16_t *settings);
void settings_brightness_bar(const dmt_view *,unsigned level,uint16_t *settings);
void settings_reset_choices(const dmt_view *,int selected,bool english);
void settings_select_render(const dmt_view *,bool classic,int page,int option,uint16_t *settings);
#endif
