#ifndef DMT_MENU_H
#define DMT_MENU_H
#include "control.h"
#include "settings.h"
/* Original menu IDs are retained so traces can be compared without a mapping. */
enum dmt_menu_state {MENU_VIEW=1,MENU_VIEW_REGULAR=2,MENU_VIEW_RECORD=3,MENU_VIEW_MONITOR=4,
 MENU_MEASUREMENT=5,MENU_HISTORY=12,MENU_SETTINGS=13,MENU_CHINESE=14,MENU_ENGLISH=15,
 MENU_LAYOUT_0=20,MENU_LAYOUT_1=21,MENU_BRIGHTNESS=22,MENU_CELSIUS=23,MENU_FAHRENHEIT=24,
 MENU_OFF_DISABLED=25,MENU_OFF_5MIN=26,MENU_OFF_15MIN=27,MENU_OFF_30MIN=28,
 MENU_RESET=29,MENU_RESET_CANCEL=30,MENU_RESET_CONFIRM=31,MENU_ABOUT=32};
typedef struct {uint8_t state,last,page;bool active;} dmt_menu;
enum dmt_menu_action {MENU_RENDER, MENU_PAGE_SETTINGS, MENU_PAGE_AUTO_OFF,
 MENU_PAGE_MEASUREMENTS, MENU_HISTORY_SCROLL, MENU_BRIGHTNESS_APPLY,
 MENU_SETTINGS_CHANGED, MENU_LAYOUT_CHANGED, MENU_MODE_SETUP, MENU_RESET_DEFAULTS,
 MENU_ENTER, MENU_EXIT};
typedef struct {void *context;void (*action)(void *,enum dmt_menu_action,uint8_t,const dmt_menu *,const dmt_control *);} dmt_menu_io;
void menu_begin(dmt_menu *,const dmt_control *,const dmt_menu_io *);
void menu_navigate(dmt_menu *,dmt_control *,const dmt_menu_io *); /* 0x164d4 */
void menu_enter_back(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *); /* 0x1598c */
void menu_select(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *); /* 0x15af6 */
bool menu_step(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *); /* 0x169d4 state policy */
#endif
