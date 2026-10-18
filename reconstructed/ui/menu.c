/* Recovered menu policy, original 0x1598c,0x15af6,0x164d4,0x169d4.
 * Legacy entries keep their observer-only ABI; rendered entries additionally
 * preserve the original ordered display operations.
 * IDs 16..19 are holes in the original switches, and endpoint rows do not wrap.
 */
#include "menu.h"
#include "menu_render.h"
#include "settings_scene.h"
static void emit(const dmt_menu_io *o,enum dmt_menu_action a,uint8_t value,const dmt_menu *m,const dmt_control *s)
{if(o && o->action)o->action(o->context,a,value,m,s);}
void menu_begin(dmt_menu *m,const dmt_control *s,const dmt_menu_io *o)
{m->active=true;emit(o,MENU_ENTER,0,m,s);}
void menu_navigate_rendered(dmt_menu *m,dmt_control *s,uint16_t *word,const dmt_menu_io *o,const dmt_menu_paint *p)
{
    static const uint8_t next[]={5,3,4,2,12,7,8,9,10,11,11,13,29,15,22,16,17,18,19,21,14,23,24,25,26,27,28,28,32,30,30,1};
    static const uint8_t prev[]={32,4,2,3,1,6,6,7,8,9,10,5,12,21,14,16,17,18,19,20,20,15,22,23,24,25,26,27,13,31,31,29};
    if(s->event!=1 && s->event!=2)return;
    uint8_t old=m->state;
    menu_paint_navigate(p,old,s->event,s->layout,word);
    if(old>=1 && old<=32)m->state=(s->event==1?next:prev)[old-1];
    if((old==15 && s->event==1) || (old==25 && s->event==2))emit(o,MENU_PAGE_SETTINGS,0,m,s);
    if(old==24 && s->event==1)emit(o,MENU_PAGE_AUTO_OFF,0,m,s);
    if((old==22 && s->event==2) || (old==13 && s->event==2))emit(o,MENU_PAGE_MEASUREMENTS,1,m,s);
    s->event=0;
}
void menu_enter_back_rendered(dmt_menu *m,dmt_control *s,uint16_t *word,const dmt_menu_io *o,const dmt_menu_paint *p)
{
    if(s->event==3){
        switch(m->state){
        case 1:m->state=2;break;
        case 5:m->state=6;break;
        case 12:emit(o,MENU_HISTORY_SCROLL,0,m,s);break;
        case 13:{uint32_t args[8]={1};if(p){view_trace(p->view,0x14408,args,0);if(p->view && p->view->status)status_scrollbar(p->view->status,1,s->layout);}emit(o,MENU_PAGE_MEASUREMENTS,1,m,s);m->state=20;break;}
        case 22:{
            uint8_t value=(uint8_t)settings_access(word,SETTINGS_FIELD_BITS_3_4,0,0)+1;
            if(p){if(s->layout)settings_brightness_image(p->view,1,m->page,value,word);else settings_brightness_bar(p->view,value,word);}
            if(value>=3)value=0;
            settings_access(word,SETTINGS_FIELD_BITS_3_4,1,value);
            emit(o,MENU_BRIGHTNESS_APPLY,value,m,s);break;
        }
        case 29:m->state=30;break;
        default:break;
        }
        /* A blocking history handler can replace event with a long-menu press. */
        if(s->event!=6)s->event=0;
    }else if(s->event==4){
        if(m->state>=2 && m->state<=4)m->state=1;
        else if(m->state>=6 && m->state<=11)m->state=5;
        else if(m->state==14 || m->state==15 || (m->state>=20 && m->state<=28)){
            menu_paint_back(p,s->layout,*word);
            m->page=1;m->state=13;emit(o,MENU_PAGE_SETTINGS,0,m,s);
        }else if(m->state==30 || m->state==31)m->state=29;
        /* Original BACK leaves event 4 pending. */
    }
}
void menu_select_rendered(dmt_menu *m,dmt_control *s,uint16_t *word,const dmt_menu_io *o,const dmt_menu_paint *p)
{
    if(s->event!=5)return;
    uint8_t id=m->state;
    menu_paint_select(p,id,s->layout,*word);
    if(id>=2 && id<=4){s->view=id-1;settings_access(word,SETTINGS_THEME,1,s->view);m->state=1;return;}
    if(id>=6 && id<=11){emit(o,MENU_MODE_SETUP,id,m,s);return;}
    switch(id){
    case 14:case 15:
        settings_language_set(word,id-14);emit(o,MENU_SETTINGS_CHANGED,id,m,s);break;
    case 20:case 21:
        if(settings_access(word,SETTINGS_LAYOUT,0,0)==(uint8_t)(21-id)){
            settings_access(word,SETTINGS_LAYOUT,1,id-20);s->layout=id-20;
            emit(o,MENU_LAYOUT_CHANGED,id,m,s);
        }
        break;
    case 22:break;
    case 23:case 24:settings_access(word,SETTINGS_TEMPERATURE,1,id-23);emit(o,MENU_SETTINGS_CHANGED,id,m,s);break;
    case 25:case 26:case 27:case 28:settings_auto_off_set(word,id-25);emit(o,MENU_SETTINGS_CHANGED,id,m,s);break;
    case 30:m->state=29;break;
    case 31:m->state=1;emit(o,MENU_RESET_DEFAULTS,0,m,s);break;
    default:return;
    }
    s->event=0;
}
bool menu_step_rendered(dmt_menu *m,dmt_control *s,uint16_t *word,const dmt_menu_io *o,const dmt_menu_paint *p)
{
    if(s->event==6){
        if(p && p->alarm)*p->alarm=255;
        menu_paint_clear(p);
        if(p){uint32_t args[8]={0};view_trace(p->view,0x14ef2,args,0);if(p->base)p->base(p->context);}
        m->state=1;m->last=0x19;m->page=1;m->active=false;s->event=0;
        if(p && p->auto_cache)*p->auto_cache=255;
        if(p && p->unit_cache)*p->unit_cache=25;
        emit(o,MENU_EXIT,0,m,s);return true;
    }
    menu_navigate_rendered(m,s,word,o,p);menu_enter_back_rendered(m,s,word,o,p);
    bool restart=s->event==5 && m->state==31;
    menu_select_rendered(m,s,word,o,p);
    /* Original defaults restore tail-calls application_loop. The cooperative
     * host returns to its scheduler without painting another menu scene. */
    if(restart)return false;
    if(m->last!=m->state){m->last=m->state;menu_paint_state(p,m,s,*word);emit(o,MENU_RENDER,m->state,m,s);}
    return false;
}
void menu_navigate(dmt_menu *m,dmt_control *s,const dmt_menu_io *o)
{menu_navigate_rendered(m,s,0,o,0);}
void menu_enter_back(dmt_menu *m,dmt_control *s,uint16_t *w,const dmt_menu_io *o)
{menu_enter_back_rendered(m,s,w,o,0);}
void menu_select(dmt_menu *m,dmt_control *s,uint16_t *w,const dmt_menu_io *o)
{menu_select_rendered(m,s,w,o,0);}
bool menu_step(dmt_menu *m,dmt_control *s,uint16_t *w,const dmt_menu_io *o)
{return menu_step_rendered(m,s,w,o,0);}
