#ifndef DMT_HISTORY_VIEW_H
#define DMT_HISTORY_VIEW_H
#include "view.h"
#include "history.h"
void history_page_render(const dmt_view *,const dmt_history *,uint8_t page,uint8_t rows,uint8_t layout,bool english,bool fahrenheit,bool draw_scrollbar);
void history_unit_update(const dmt_view *,uint8_t unit,uint16_t y,uint8_t *previous_unit,uint16_t *previous_y,bool fahrenheit);
void history_strip_render(const dmt_view *,const char *text,const char *previous,uint8_t row,bool wrapped,uint8_t unit,uint8_t *previous_unit,uint16_t *previous_y,bool fahrenheit);
#endif
