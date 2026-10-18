#ifndef DMT_MENU_RENDER_H
#define DMT_MENU_RENDER_H
#include "menu.h"
#include "view.h"
#include "history.h"
/* Application-owned data and the original 14ef2 external-call boundary.
 * No settings writes, resource state or framebuffer are hidden here. */
typedef struct {
 const dmt_view *view;
 const uint16_t *thresholds;
 const dmt_history *history;
 void *context;
 void (*base)(void *);
 uint8_t *alarm,*auto_cache,*unit_cache; /* 169d4/fdf6 exit stores */
} dmt_menu_paint;
void menu_paint_begin(const dmt_menu_paint *,uint8_t layout,uint16_t settings);
void menu_paint_navigate(const dmt_menu_paint *,uint8_t state,uint8_t event,uint8_t layout,uint16_t *settings);
void menu_paint_back(const dmt_menu_paint *,uint8_t layout,uint16_t settings);
void menu_paint_select(const dmt_menu_paint *,uint8_t state,uint8_t layout,uint16_t settings);
void menu_paint_state(const dmt_menu_paint *,const dmt_menu *,const dmt_control *,uint16_t settings);
void menu_paint_clear(const dmt_menu_paint *);
/* Rendered counterparts preserve the existing observer-only API and its ABI. */
void menu_navigate_rendered(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *,const dmt_menu_paint *);
void menu_enter_back_rendered(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *,const dmt_menu_paint *);
void menu_select_rendered(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *,const dmt_menu_paint *);
bool menu_step_rendered(dmt_menu *,dmt_control *,uint16_t *,const dmt_menu_io *,const dmt_menu_paint *);
void menu_threshold_all(const dmt_view *,uint8_t layout,bool english); /* 1a336 */
#endif
