/* Recovered orchestration from 0x908a,0x8ed4,0xc248,0x14cd6,0xc320.
 * Blocking menu/editor/charging loops are represented as resumable states so
 * host tests can schedule input deterministically. There is still one slot
 * for button events and one for saved ASIC events, as in the original.
 * The explicit two-key diagnostic entry replays the recovered wire bytes;
 * its0xa0 command has no established ASIC meaning.
 */
#include "application.h"
#include "format.h"
#include "monitor.h"
#include "history_view.h"
#include "menu_render.h"
bool application_startup(const dmt_startup_io *o)
{
    /* Fail before any side effect if the application dependency set is absent. */
    if(!o || !o->priority_group || !o->clock_update || !o->delay_init ||
       !o->debug_uart || !o->measurement_uart || !o->buttons || !o->battery_inputs ||
       !o->display || !o->resources || !o->timer || !o->loop)return false;
    o->priority_group(o->context,2);o->clock_update(o->context);o->delay_init(o->context);
    o->debug_uart(o->context,115200);o->measurement_uart(o->context,9600);
    o->buttons(o->context);o->battery_inputs(o->context);o->display(o->context);
    o->resources(o->context);o->timer(o->context,9999,14399);o->loop(o->context);
    return true;
}
typedef struct {dmt_application *a;const dmt_application_io *o;} context;
#if DMT_SIMPLE_SCREEN
/* The image-free simple screen replaces the Regular view when enabled. */
static bool simple_on(const dmt_application *a,const dmt_application_io *o,dmt_simple_config *cfg)
{
    if(!o->canvas || !o->screen || a->control.view!=1)return false;
    simple_config_load(a->settings.halfwords,cfg);return cfg->on;
}
#endif
static void gap(dmt_application *a,const dmt_application_io *o,uint32_t address)
{
    if(o->screen && (address==0x1511a || address==0x14ef2)){
        if(o->battery_read)a->battery_percent=o->battery_read(o->context);
#if DMT_SIMPLE_SCREEN
        dmt_simple_config cfg;
        if(simple_on(a,o,&cfg)){
            simple_base(o->canvas,o->screen,&cfg,&a->simple,&a->control,a->frame,*settings_image_word(&a->settings),a->battery_percent,address==0x1511a);
            return;
        }
#endif
        if(address==0x1511a)screen_initial(o->screen,&a->control,a->frame,*settings_image_word(&a->settings),a->battery_percent);
        else screen_base(o->screen,&a->control,a->frame,*settings_image_word(&a->settings),a->battery_percent);
        return;
    }
    if(o->view && address==0x14578){history_page_render(o->view,&a->history,a->browser.page,history_browser_rows(&a->browser),a->control.layout,(*settings_image_word(&a->settings)&7)!=0,(*settings_image_word(&a->settings)&32)!=0,true);return;}
    if(!o->unresolved || !o->unresolved(o->context,address,a)){
        ++a->unresolved_calls;a->last_unresolved_address=address;
    }
}
static void remove_logo(void *v)
{context *c=v;gap(c->a,c->o,0xde12);}
void application_reset_history_strip(dmt_application *a,const dmt_application_io *o)
{
    a->history_row=1;a->history_wrapped=0;
    if(o->screen)o->screen->history_unit_y=0;
}
void application_invalidate_display(dmt_application *a,const dmt_application_io *o)
{
    a->alarm=255;a->control.menu_last=25;a->menu.last=25;
    if(o->screen){o->screen->unit_cache=25;o->screen->auto_cache=255;}
}
static void import_history(dmt_application *a)
{
    a->history.count=a->settings.halfwords[16];
    if(a->history.count>30)a->history.count=30; /* reconstruction bounds guard */
    for(unsigned i=0;i<30;++i){a->history.records[i].metadata=a->settings.halfwords[17+2*i];a->history.records[i].magnitude=a->settings.halfwords[18+2*i];}
}
static void export_history(dmt_application *a)
{
    a->settings.halfwords[16]=a->history.count;
    for(unsigned i=0;i<30;++i){a->settings.halfwords[17+2*i]=a->history.records[i].metadata;a->settings.halfwords[18+2*i]=a->history.records[i].magnitude;}
}
static void apply_settings(dmt_application *a)
{
    uint16_t *word=settings_image_word(&a->settings);
    unsigned view=settings_access(word,SETTINGS_THEME,0,1);
    if(view<1 || view>3){view=1;settings_access(word,SETTINGS_THEME,1,1);}
    a->control.view=view;a->control.layout=settings_access(word,SETTINGS_LAYOUT,0,0)==1;
    a->power.threshold_seconds=settings_auto_off_seconds(*word);
}
static void initial_screen(dmt_application *a,const dmt_application_io *o)
{
    if(o->reading)reading_reset_extrema(o->reading);
    apply_settings(a);gap(a,o,0x1511a); /* application_show_initial_screen */
    a->control.mode=a->control.view==1?0x14:0x0c;a->control.event=a->control.view==1?5:3;
    if(a->control.view!=1 && o->delay_ms)o->delay_ms(o->context,500);
}
/* c248's record check and 15168/1511a dispatch. The factory reset re-enters
 * it as well: 0x169d4 restores defaults (record word 0 becomes 1) and
 * tail-calls application_loop 0x8ed4, so c248 restores defaults again and
 * shows the language selection before the measurement screen. */
static void settings_entry(dmt_application *a,const dmt_application_io *o)
{
    if(a->settings.halfwords[0]!=0){
        context restore={a,o};settings_image_restore_defaults(&a->settings,&a->settings_loaded,&restore,remove_logo);
        a->run=DMT_LANGUAGE_SELECT;a->language_choice=1;
        if(o->backlight)o->backlight(o->context,settings_backlight_compare(*settings_image_word(&a->settings)));
        if(o->status_view)status_language_choice(o->status_view,1,true,false);else gap(a,o,0x15168);
    }else{a->run=DMT_RUNNING;initial_screen(a,o);}
    import_history(a);
}
void application_init(dmt_application *a,const dmt_settings_image *image,const dmt_application_io *o)
{
    *a=(dmt_application){0};a->menu.state=1;a->menu.last=0x19;a->menu.page=1;
    a->rx_suppressed=1;a->event_consumed=1;a->history_row=1;a->alarm=0xff;a->last_ui_event=0xff;a->control.menu_last=0x19;
    if(image)a->settings=*image;
    else if(!settings_image_load(&a->settings,o->internal_flash)){
        a->run=DMT_STOPPED;gap(a,o,0xd928);return;
    }
    a->settings_loaded=1;
    settings_entry(a,o);
#if DMT_LAST_MODE
    if(a->run==DMT_RUNNING)last_mode_restore(&a->control,o->last_mode,o->send,o->context);
#endif
}
static void menu_base(void *v)
{context *c=v;gap(c->a,c->o,0x14ef2);}
static dmt_menu_paint menu_paint_for(context *c)
{return (dmt_menu_paint){.view=c->o->screen?&c->o->screen->view:c->o->view,.thresholds=c->a->settings.halfwords+9,.history=&c->a->history,.context=c,.base=menu_base,.alarm=&c->a->alarm,.auto_cache=c->o->screen?&c->o->screen->auto_cache:0,.unit_cache=c->o->screen?&c->o->screen->unit_cache:0};}
static void menu_action(void *v,enum dmt_menu_action action,uint8_t value,const dmt_menu *menu,const dmt_control *s)
{
    context *c=v;dmt_application *a=c->a;const dmt_application_io *o=c->o;
    if(action==MENU_MODE_SETUP){
        if(threshold_editor_begin(&a->editor,a->settings.halfwords+9,value)){
            a->control.event=0;a->run=DMT_EDIT_THRESHOLD;
            threshold_editor_step(&a->editor,a->settings.halfwords+9,&a->control.event);
            if(o->canvas)threshold_editor_draw(&a->editor,o->canvas,a->control.layout,true);
        }
    }else if(action==MENU_HISTORY_SCROLL){
        history_browser_begin(&a->browser,(uint8_t)a->history.count);a->run=DMT_BROWSE_HISTORY;
    }else if(action==MENU_BRIGHTNESS_APPLY){if(o->backlight)o->backlight(o->context,settings_backlight_compare(*settings_image_word(&a->settings)));}
    else if(action==MENU_SETTINGS_CHANGED && value>=25 && value<=28)a->power.threshold_seconds=settings_auto_off_seconds(*settings_image_word(&a->settings));
    else if(action==MENU_RESET_DEFAULTS){
#if DMT_LAST_MODE
        (void)last_mode_forget(o->last_mode);
#endif
#if DMT_THEMES
        /* Factory reset returns to the original artwork; installed themes stay. */
        if(o->themes)(void)o->themes->select(o->themes->context,0);
#endif
        settings_image_restore_defaults(&a->settings,&a->settings_loaded,c,remove_logo);settings_entry(a,o);
    }else if(action==MENU_EXIT){
        a->run=DMT_RUNNING;a->control.menu_last=0x19;a->last_ui_event=0xff;
        /* The enclosing button handler resumes after169d4 returns: recording
         * resets extrema, while monitor resets only its own selector/state. */
        if(a->menu_origin_view==2){a->control.record_selector[0]=0;if(o->reading)reading_reset_extrema(o->reading);}
        if(a->menu_origin_view==3){a->control.monitor_state=0;a->control.monitor_selector=0;}
        control_after_menu(&a->control);application_invalidate_display(a,o);
    }
    if(action==MENU_ENTER){
        if(o->screen || o->view){dmt_menu_paint paint=menu_paint_for(c);menu_paint_begin(&paint,s->layout,*settings_image_word(&a->settings));}
    }
    if(o->menu_action)o->menu_action(o->context,action,value,menu,s);
    else if(!o->screen && (action==MENU_RENDER || action==MENU_ENTER))gap(a,o,0x169d4);
}
static void send(void *v,const uint8_t p[10]){context *c=v;if(c->o->send)c->o->send(c->o->context,p);else gap(c->a,c->o,0x97c4);}
static void diagnostic_delay(void *v,uint32_t ms)
{context *c=v;if(c->o->delay_ms)c->o->delay_ms(c->o->context,ms);else gap(c->a,c->o,0x1a64c);}
bool application_boot(dmt_application *a,const dmt_settings_image *image,const dmt_application_io *o)
{
    if(!o->diagnostic_buttons || !o->diagnostic_buttons->pin_high)return false;
    const dmt_button_io *keys=o->diagnostic_buttons;
    if(keys->pin_high(keys->context,0x400) || keys->pin_high(keys->context,0x800)){
        application_init(a,image,o);return true;
    }
    *a=(dmt_application){0};a->run=DMT_DIAGNOSTIC;a->rx_suppressed=1;a->event_consumed=1;
    /* Preserved initialized SRAM settings and initial view, not a device read. */
    settings_image_defaults(&a->settings);a->control.view=1;
    if(o->screen)o->screen->normal=true;
    context c={a,o};
    if(o->backlight)o->backlight(o->context,0);else gap(a,o,0x7f66);
    diagnostic_delay(&c,1000);measurement_diagnostic_entry(&c,send,diagnostic_delay);
    return true;
}
static void diagnostic_step(dmt_application *a,const dmt_application_io *o)
{
    context c={a,o};
    if(o->diagnostic_buttons)measurement_forward_button(&a->control.event,o->diagnostic_buttons,&c,send);
    else gap(a,o,0x97f8);
    if(a->rx_suppressed)return;
    if(!a->event_consumed)a->event_consumed=1;
    else{
        control_parse(&a->control,a->frame);
        uint32_t args[8]={100,100,100,50,32,0xfd40,0};
        const dmt_view *v=o->screen?&o->screen->view:o->view;
        view_trace(v,0x8672,args,a->control.text);
        if(o->canvas)graphics_text_box(o->canvas,100,100,100,50,32,0xfd40,a->control.text,false);else gap(a,o,0x8672);
        args[0]=200;args[1]=110;view_trace(v,0xc11a,args,0);
        if(o->status_view)status_unit(o->status_view,200,110,a->control.unit,&a->alarm);else gap(a,o,0xc11a);
    }
    a->rx_suppressed=0; /* Original diagnostic loop renders the frame repeatedly. */
}
static bool longpress(void *v,uint8_t e){context *c=v;return c->o->longpress && c->o->longpress(c->o->context,e);}
static uint8_t overload(const char *p)
{if(p[2]=='.')return p[3]=='0' && p[4]=='L';return p[2]=='0' && p[3]=='.' && p[4]=='L'?2:0;}
static void control_effect(void *v,enum dmt_effect effect,const dmt_control *s)
{
    context *c=v;dmt_application *a=c->a;const dmt_application_io *o=c->o;
    if(effect==DMT_EFFECT_MENU_OPEN){
        a->run=DMT_MENU;a->menu_origin_view=s->view;
        if(s->view==2)application_reset_history_strip(a,o);
        dmt_menu_io io={c,menu_action};menu_begin(&a->menu,s,&io);
        dmt_menu_paint paint=menu_paint_for(c);
        menu_step_rendered(&a->menu,&a->control,settings_image_word(&a->settings),&io,&paint);
    }else if(effect==DMT_EFFECT_HISTORY_APPEND){
        if(s->mode==0x12 && settings_access(settings_image_word(&a->settings),SETTINGS_TEMPERATURE,0,0)==1)
            dmt_format(a->control.text,sizeof a->control.text,"%4d",(int)((double)s->value*1.8+32));
        /* 0x12c78 keeps an eight-row highlight and a previous-text cache. */
        if(o->screen){
            uint32_t args[8]={0};view_trace(&o->screen->view,0x12c78,args,s->text);
            dmt_view quiet=o->screen->view;quiet.trace=0;
            history_strip_render(&quiet,s->text,a->previous_history_text,a->history_row,a->history_wrapped,s->unit,&o->screen->history_unit,&o->screen->history_unit_y,(*settings_image_word(&a->settings)&32)!=0);
        }
        else gap(a,o,0x12c78);
        a->history_wrapped=a->history_row==8;
        a->history_row=a->history_row<8?a->history_row+1:1;
        for(unsigned i=0;i<sizeof a->previous_history_text;++i)a->previous_history_text[i]=s->text[i];
        dmt_history_record r;
        if(history_encode(s->mode,s->unit,s->value,overload(s->text),&r)){history_push(&a->history,r);export_history(a);}
    }else if(effect==DMT_EFFECT_BUZZER_OFF){if(o->buzzer)o->buzzer(o->context,false);}
    else if(effect==DMT_EFFECT_BEEP){
        if(o->buzzer)o->buzzer(o->context,true);
        if(o->delay_ms)o->delay_ms(o->context,250);
        if(o->buzzer)o->buzzer(o->context,false);
    }else if(effect==DMT_EFFECT_READING){
#if DMT_SIMPLE_SCREEN
        dmt_simple_config cfg;
        if(simple_on(a,o,&cfg))simple_reading(o->canvas,&cfg,&a->simple,&a->control,a->frame,*settings_image_word(&a->settings));
        else
#endif
        if(o->screen)screen_reading(o->screen,&a->control,a->frame,*settings_image_word(&a->settings));
        else if(o->reading && o->view)reading_render(o->reading,&a->control,a->frame,*settings_image_word(&a->settings),o->view);
        else if(a->control.numeric){
            float value=s->view==1?measurement_relative_value(s):s->value;
            if(s->mode==0x12 && settings_access(settings_image_word(&a->settings),SETTINGS_TEMPERATURE,0,0)==1)value=(float)((double)value*1.8+32);
            measurement_format_value(a->control.text,sizeof a->control.text,value,s->mode,s->unit);
        }
        if(s->view==3 && o->view && !o->view->monitor_alarm){monitor_alarm_render(o->view,a->settings.halfwords+9,s->mode,s->value,(*settings_image_word(&a->settings)&32)!=0,s->layout,&a->alarm);}
        else if(s->view==3 && threshold_alarm(a->settings.halfwords+9,s->mode,s->value,(*settings_image_word(&a->settings)&32)!=0,&a->alarm)){
            if(o->buzzer)o->buzzer(o->context,a->alarm!=0);
            gap(a,o,0x13264); /* image triplets remain a renderer boundary */
        }
    }
#if DMT_SIMPLE_SCREEN
    dmt_simple_config simple_cfg;
    if(simple_on(a,o,&simple_cfg)){
        if(effect==DMT_EFFECT_SCREEN_RESTORE)simple_base(o->canvas,o->screen,&simple_cfg,&a->simple,&a->control,a->frame,*settings_image_word(&a->settings),a->battery_percent,false);
        else if(effect!=DMT_EFFECT_READING)simple_event(o->canvas,&simple_cfg,&a->simple,s);
    }else{
#endif
    if(o->screen)screen_event(o->screen,s,*settings_image_word(&a->settings),effect,a->frame[3]==3);
    if(o->screen && effect==DMT_EFFECT_SCREEN_RESTORE){view_fill(&o->screen->view,0,0,319,239,0);screen_base(o->screen,&a->control,a->frame,*settings_image_word(&a->settings),a->battery_percent);}
#if DMT_SIMPLE_SCREEN
    }
#endif
    if(o->effect)o->effect(o->context,effect,s);
    else if(effect==DMT_EFFECT_MONITOR_CONFIG){if(o->view)monitor_threshold_render(o->view,a->settings.halfwords+9,s->mode,s->layout,(*settings_image_word(&a->settings)&7)!=0,(*settings_image_word(&a->settings)&32)!=0);else gap(a,o,0x12d5a);}
    else if(!o->screen && !(effect==DMT_EFFECT_READING && o->reading && o->view) && effect!=DMT_EFFECT_MENU_OPEN && effect!=DMT_EFFECT_HISTORY_APPEND && effect!=DMT_EFFECT_BEEP && effect!=DMT_EFFECT_BUZZER_OFF)gap(a,o,0x9caa);
}
void application_receive_idle(dmt_application *a,const uint8_t b[13])
{
    for(unsigned i=0;i<13;++i)a->frame[i]=b[i];
    a->rx_suppressed=measurement_frame_is_blank(b);
    if(b[2]==0xfd){for(unsigned i=0;i<13;++i)a->saved_event[i]=b[i];a->event_consumed=0;}
}
void application_step(dmt_application *a,const dmt_application_io *o)
{
    context c={a,o};dmt_control_io control_io={&c,send,longpress,control_effect};dmt_menu_io menu_io={&c,menu_action};
    if(a->run==DMT_STOPPED)return;
    if(a->run==DMT_DIAGNOSTIC){diagnostic_step(a,o);return;}
    if(a->run==DMT_LANGUAGE_SELECT){
        if(a->control.event==3 || a->control.event==4){a->language_choice=a->language_choice==1?2:1;a->control.event=0;if(o->status_view)status_language_choice(o->status_view,a->language_choice,false,false);}
        if(a->control.event==5){if(o->status_view)status_language_choice(o->status_view,a->language_choice,false,true);settings_language_set(settings_image_word(&a->settings),a->language_choice==2);a->settings.halfwords[0]=0;a->control.event=0;a->run=DMT_RUNNING;initial_screen(a,o);}
        return;
    }
    if(a->run==DMT_CHARGING){if(!o->charging || !o->charging(o->context)){a->run=DMT_RUNNING;a->control.monitor_state=0;a->control.monitor_selector=0;a->control.record_selector[0]=0;initial_screen(a,o);}return;}
    if(a->run==DMT_EDIT_THRESHOLD){
        if(threshold_editor_step(&a->editor,a->settings.halfwords+9,&a->control.event))a->run=DMT_MENU;
        if(o->canvas)threshold_editor_draw(&a->editor,o->canvas,a->control.layout,a->editor.active);
        return;
    }
    if(a->run==DMT_BROWSE_HISTORY){
        uint8_t old_page=a->browser.page;
        if(history_browser_step(&a->browser,&a->control.event)){
            if(o->view)view_fill(o->view,a->control.layout?315:317,a->control.layout?5:11,a->control.layout?317:319,a->control.layout?234:228,0);
            if(a->control.event!=6)a->control.event=0;
            a->run=DMT_MENU;
        }else if(a->browser.dirty){if(o->view && old_page!=a->browser.page)view_fill(o->view,132,51,315,a->control.layout?240:215,0);gap(a,o,0x14578);a->browser.dirty=false;}
        return;
    }
#if DMT_THEMES
    if(a->run==DMT_THEME_MANAGER){
        /* Leaving goes through the regular menu exit (event 6). */
        if(theme_menu_step(&a->themes,&a->control.event,o->themes,o->canvas)){a->run=DMT_MENU;a->control.event=6;}
        else return;
    }
    if(a->run==DMT_MENU && o->themes && o->canvas && a->menu.state==MENU_ABOUT && a->control.event==3){
        a->control.event=0;a->run=DMT_THEME_MANAGER;theme_menu_open(&a->themes,o->themes,o->canvas);return;
    }
#endif
    if(a->run==DMT_MENU){
        dmt_menu_paint paint=menu_paint_for(&c);
        menu_step_rendered(&a->menu,&a->control,settings_image_word(&a->settings),&menu_io,&paint);return;
    }
    if(o->screen && (a->control.event==1 || a->control.event==3 || a->control.event==4 || (a->control.view==1 && a->control.event==5)))o->screen->unit_cache=25;
    control_buttons(&a->control,&control_io);
    if(a->run==DMT_MENU || a->run==DMT_EDIT_THRESHOLD)return;
    if(a->rx_suppressed==0){
        if(a->event_consumed==0){for(unsigned i=0;i<16;++i)a->frame[i]=a->saved_event[i];a->event_consumed=1;a->last_ui_event=0xff;if(o->screen)o->screen->auto_cache=0xff;}
        if(!a->control.hold)control_parse(&a->control,a->frame);
        control_event(&a->control,a->frame,&control_io);a->rx_suppressed=1;
#if DMT_LAST_MODE
        if(a->frame[0]==0x5a && a->frame[1]==0xa5 && a->frame[2]==0xfd && a->frame[3]==a->control.mode)
            (void)last_mode_save(o->last_mode,a->control.view,a->control.mode);
#endif
    }
    if(o->charging && o->charging(o->context)){a->run=DMT_CHARGING;if(o->status_view)status_charging(o->status_view,settings_language_prefix(*settings_image_word(&a->settings)));else gap(a,o,0x14cd6);application_invalidate_display(a,o);return;}
    if(a->power.battery_due==1){a->power.battery_due=0;if(a->control.mode!=0x13 && a->control.mode!=9){
        if(o->battery_read)a->battery_percent=o->battery_read(o->context);
#if DMT_SIMPLE_SCREEN
        dmt_simple_config cfg;
        if(simple_on(a,o,&cfg))simple_battery(o->canvas,&cfg,&a->simple,a->battery_percent);
        else
#endif
        if(o->status_view)status_battery(o->status_view,a->battery_percent,a->control.layout);else gap(a,o,a->control.layout?0x14d62:0x14e48);
    }}
}
void application_beep_500(const dmt_application_io *o)
{
    if(o->buzzer)o->buzzer(o->context,true);
    if(o->delay_ms)o->delay_ms(o->context,500);
    if(o->buzzer)o->buzzer(o->context,false);
}
void application_power_down(dmt_application *a,const dmt_application_io *o)
{
    application_beep_500(o);
    if(o->canvas)graphics_fill(o->canvas,0,0,319,239,0);else gap(a,o,0x876a);
    if(o->backlight)o->backlight(o->context,100);
    export_history(a);
#if DMT_LAST_MODE
    /* The mode in use at power-off, independent of the measurement IC's ACK. */
    (void)last_mode_save(o->last_mode,a->control.view,a->control.mode);
#endif
    if(settings_image_commit(&a->settings,o->internal_flash,4)!=4)gap(a,o,0xd87c);
#if DMT_LAST_MODE
    (void)last_mode_commit(o->last_mode);
#endif
    if(o->power_latch)o->power_latch(o->context,false);
    a->run=DMT_STOPPED;a->power.shutdown=true;
}
static int battery(void *v){context *c=v;return c->o->battery_level?c->o->battery_level(c->o->context):c->o->battery_read?c->o->battery_read(c->o->context):100;}
static bool charging(void *v){context *c=v;return c->o->charging && c->o->charging(c->o->context);}
static void low_battery(void *v){context *c=v;if(c->o->status_view)status_low_battery_countdown(c->o->status_view,settings_language_prefix(*settings_image_word(&c->a->settings)),*settings_image_word(&c->a->settings)&7);else gap(c->a,c->o,0x14c42);application_power_down(c->a,c->o);}
static void shutdown(void *v){context *c=v;application_power_down(c->a,c->o);}
void application_timer(dmt_application *a,const dmt_application_io *o)
{context c={.a=a,.o=o};dmt_power_io io={&c,battery,charging,low_battery,shutdown};power_timer_update(&a->power,*settings_image_word(&a->settings),&io);}
