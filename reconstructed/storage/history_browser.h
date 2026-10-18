#ifndef DMT_HISTORY_BROWSER_H
#define DMT_HISTORY_BROWSER_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {uint8_t page,pages,count;bool active,dirty;} dmt_history_browser;
void history_browser_begin(dmt_history_browser *,uint8_t count);
bool history_browser_step(dmt_history_browser *,uint8_t *event);
uint8_t history_browser_rows(const dmt_history_browser *);
uint8_t history_browser_first(const dmt_history_browser *);
#endif
