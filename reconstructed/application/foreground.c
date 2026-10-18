/* One normal-operation iteration of 0x8ed4, after startup/special two-key handling.
 * Unknown UI/charge routines remain explicit injected operations with original
 * addresses. No substitute implementation or complete application is claimed.
 * Entry and diagnostic iterations below retain the other8ed4 branches through
 * explicit original callee contracts, with host yields at polling boundaries.
 */
#include "foreground.h"
void foreground_loop(dmt_foreground_state *s,uint8_t *event,uint8_t *normal,const dmt_foreground_entry_ops *entry,const dmt_foreground_ops *run,const dmt_diagnostic_ops *diag)
{
    if(foreground_enter(s,event,normal,entry))for(;;)(void)foreground_diagnostic_iteration(s,diag);
    else for(;;)foreground_iteration(s,run);
}
bool foreground_enter(dmt_foreground_state *s,uint8_t *event,uint8_t *normal,const dmt_foreground_entry_ops *o)
{
    if(!o->pin_high(o->context,0x400) && !o->pin_high(o->context,0x800)){
        *normal=1;o->backlight(o->context,100,4799,0);o->delay_ms(o->context,1000);
        o->diagnostic_entry(o->context);return true;
    }
    o->load_settings(o->context);o->initial_screen(o->context);
    if(s->view==1){s->mode=0x14;*event=5;}
    else{s->mode=0x0c;*event=3;o->delay_ms(o->context,500);}
    return false;
}
bool foreground_diagnostic_iteration(dmt_foreground_state *s,const dmt_diagnostic_ops *o)
{
    o->forward_button(o->context);
    if(s->rx_suppressed)return false;
    if(!s->event_consumed)s->event_consumed=1;
    else{
        o->parse(o->context,s->frame);
        const char *text=o->text(o->context);
        o->draw_text(o->context,100,100,100,50,32,0xfd40,text);
        o->draw_unit(o->context,200,110);
    }
    /*8f70 writes1, then8f74 jumps to8f3a which immediately writes0.
     * A diagnostic frame is rendered again without requiring a new IRQ. */
    s->rx_suppressed=0;return true;
}
void foreground_iteration(dmt_foreground_state *s,const dmt_foreground_ops *o)
{
    if(s->view==1)o->regular_buttons(o->context);
    else if(s->view==2)o->record_buttons(o->context);
    else if(s->view==3)o->monitor_buttons(o->context);
    if(s->rx_suppressed==0){
        if(s->event_consumed==0){
            /* Preserve the original sixteen-byte copy, including three stale bytes. */
            for(unsigned i=0;i<16;++i)s->frame[i]=s->saved_event[i];
            s->event_consumed=1;s->last_ui_event=0xff;
        }
        if(s->hold==0)o->parse_measurement(o->context,s->frame);
        if(s->view==1)o->regular_event(o->context);
        else if(s->view==2)o->record_event(o->context);
        else if(s->view==3)o->monitor_event(o->context);
        s->rx_suppressed=1;
    }
    o->charge_screen_poll(o->context);
    if(s->battery_due==1){
        s->battery_due=0;
        if(s->mode!=0x13 && s->mode!=9){
            s->battery_percent=o->battery_read(o->context);
            if(s->layout==0)o->battery_layout_0(o->context);
            else o->battery_layout_1(o->context);
        }
    }
}
