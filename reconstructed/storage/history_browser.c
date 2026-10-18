/* 0x14a92 policy. Original partial last-page calculation uses count % 5,
 * including zero rows at counts 5,10,15,20,25. Count 30 is a separate full-page
 * branch. Preserve this visible quirk rather than assuming ceil(count/5).
 */
#include "history_browser.h"
void history_browser_begin(dmt_history_browser *b,uint8_t count)
{
    b->page=1;b->pages=count>25?6:count>20?5:count>15?4:count>10?3:count>5?2:1;
    b->count=count;b->active=true;b->dirty=true;
}
bool history_browser_step(dmt_history_browser *b,uint8_t *event)
{
    if(*event==4 || *event==6){b->active=false;return true;}
    if(*event==1){if(b->page<b->pages)++b->page;*event=0;b->dirty=true;}
    else if(*event==2){if(b->page>1)--b->page;*event=0;b->dirty=true;}
    return false;
}
uint8_t history_browser_rows(const dmt_history_browser *b)
{return b->count>=30 || b->page!=b->pages?5:b->count%5;}
uint8_t history_browser_first(const dmt_history_browser *b){return (b->page-1)*5+1;}
