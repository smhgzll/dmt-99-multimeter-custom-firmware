/* TIM3 IRQ body 0x7780, after status/clear. At 144MHz, ARR9999 PSC14399
 * nominally fires once per second. Original waits 21 updates but adds only 20
 * idle seconds; shutdown compares strictly > and the uint16 counter wraps.
 */
#include "power.h"
void power_timer_update(dmt_power *s,uint16_t settings,const dmt_power_io *o)
{
    s->ticks=(uint8_t)(s->ticks+1);
    if(s->ticks<=20)return;
    if(o->battery_percent(o->context)<1 && !o->charging(o->context)){
        s->shutdown=true;o->low_battery_countdown(o->context);return;
    }
    s->battery_due=1;s->ticks=0;
    if(((settings>>6)&3)!=0){
        s->idle_seconds=(uint16_t)(s->idle_seconds+20);
        if(s->threshold_seconds<s->idle_seconds){s->shutdown=true;o->power_down(o->context);}
    }
}
