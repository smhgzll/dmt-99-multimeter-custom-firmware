#ifndef DMT_MENU_SCENE_H
#define DMT_MENU_SCENE_H
#include "view.h"
void menu_category_compact(const dmt_view *,bool english,int initial,int selected,int previous);
void menu_category_large(const dmt_view *,int initial,int selected,int previous);
void menu_choice_compact(const dmt_view *,bool english,int initial,int selected,int previous);
void menu_choice_large(const dmt_view *,int initial,int selected,int previous);
void menu_threshold_hint(const dmt_view *,bool compact,bool english,int selected,unsigned row);
void menu_threshold_images(const dmt_view *,bool english,int selected,unsigned row);
#endif
