/* State effects of 0xa910,0xaed0,0xb29e; resource layout belongs to UI scenes.
 * Range changes arrive as new unit flags in display frames, not a host-side ADC
 * algorithm. Monitor configuration is an explicit effect of its group changes.
 */
#include "control.h"
static void emit(const dmt_control_io *o,enum dmt_effect e,const dmt_control *s)
{if(o->effect)o->effect(o->context,e,s);}
void control_event(dmt_control *s,const uint8_t b[13],const dmt_control_io *o)
{
    if(b[2]!=0xfd){
        if(s->view!=1 || b[0]==0x5a){
            emit(o,DMT_EFFECT_READING,s);
            if(s->view==1 && s->mode==0x14)emit(o,DMT_EFFECT_AUTO_READING,s);
        }
        return;
    }
    uint8_t event=b[3];
    if(s->view==1 && (s->mode==9 || s->mode==0x13) && event!=0x13)
        emit(o,DMT_EFFECT_SCREEN_RESTORE,s);
    if(event!=3 && s->hold==1){s->hold=0;emit(o,DMT_EFFECT_HOLD_OFF,s);}
    if(event==3){s->hold=!s->hold;emit(o,s->hold?DMT_EFFECT_HOLD_ON:DMT_EFFECT_HOLD_OFF,s);return;}
    bool recognized=false;
    if(s->view==1)recognized=event>=9 && event<=0x16;
    else if(s->view==2)recognized=(event>=0x0a && event<=0x12) || event==0x15 || event==0x16;
    else if(s->view==3)recognized=event==0x0b || event==0x0c || event==0x0d || event==0x10 || event==0x15 || event==0x12;
    if(recognized){
        bool configure=false;
        if(s->view==3){
            if(event==0x0c || event==0x0d)configure=!s->monitor_state || (s->mode!=0x0c && s->mode!=0x0d);
            if(event==0x10 || event==0x15)configure=!s->monitor_state || (s->mode!=0x10 && s->mode!=0x15);
            if(event==0x12)configure=s->mode!=0x12;
            if(configure && event!=0x12)s->monitor_state=1;
        }
        s->mode=event;
        if(configure)emit(o,DMT_EFFECT_MONITOR_CONFIG,s);
        emit(o,DMT_EFFECT_MODE_CHANGED,s);
    }
}
