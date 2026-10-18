/* Button policies 0xb5c8,0xb9e0,0xbc90. The ASIC owns range selection;
 * these selectors reproduce commands observable on the host UART only.
 * Menu entry is made cooperative: the caller suspends measurement processing
 * until menu exit, matching the original blocking menu_dispatch loop.
 */
#include "control.h"
static void effect(const dmt_control_io *io,enum dmt_effect e,dmt_control *s)
{ if(io->effect)io->effect(io->context,e,s); }
static uint8_t cycle(uint8_t *p,uint8_t max)
{ *p=(uint8_t)(*p+1);if(*p>max)*p=1;return *p; }
static uint8_t pick(const uint8_t *table,uint8_t index,uint8_t fallback)
{return index?table[index-1]:fallback;}
static bool relative_mode(uint8_t m)
{ return m==0x0c || m==0x0d || m==0x10 || m==0x11 || m==0x15 || m==0x16 || m==0x0a; }
void control_after_menu(dmt_control *s)
{
    if(s->view==1)s->event=5;
    else{s->record_selector[0]=0;s->event=3;}
}
void control_buttons(dmt_control *s,const dmt_control_io *io)
{
    static const uint8_t primary[]={0x0c,0x0d,0x0b,0x0a,0x0e};
    static const uint8_t current[]={0x11,0x10,0x16,0x15};
    static const uint8_t other[]={0x12,0x0f,9,0x13};
    /* Exact 0x1e82c template: byte 9 remains 0x11 on command-zero paths. */
    uint8_t packet[10]={0xaa,0x55,5,0,0,0,0,0,0,0x11};
    uint8_t event=s->event,cmd=0;
    if(s->view<1 || s->view>3)return;
    if(event==6){
        s->event=0;
        if(s->view==1){s->menu_last=0x19;s->mode=0;}
        if(s->view==3)effect(io,DMT_EFFECT_BUZZER_OFF,s);
        effect(io,DMT_EFFECT_MENU_OPEN,s);return;
    }
    if(s->view==1){
        uint8_t *p=s->regular_selector;
        switch(event){
        case 3:cmd=pick(primary,cycle(p,5),0);p[1]=p[2]=p[3]=0;s->event=0;s->menu_last=0x19;break;
        case 4:cmd=pick(current,cycle(p+1,4),0x15);p[0]=p[2]=p[3]=0;s->event=0;s->menu_last=0x19;break;
        case 1:cmd=pick(other,cycle(p+2,4),0);p[0]=p[1]=p[3]=0;s->event=0;s->menu_last=0x19;break;
        case 5:cycle(p+3,12);cmd=0x14;p[0]=p[1]=p[2]=0;s->event=0;s->menu_last=0x19;break;
        case 2:
            s->event=0;
            if(io->longpress && io->longpress(io->context,2)){
                if(relative_mode(s->mode)){
                    s->relative=!s->relative;
                    if(s->relative){s->relative_mode=s->mode;s->relative_unit=s->unit;s->relative_value=s->value;}
                    effect(io,s->relative?DMT_EFFECT_RELATIVE_ON:DMT_EFFECT_RELATIVE_OFF,s);
                    effect(io,DMT_EFFECT_BEEP,s);
                }
            }else if(s->mode!=0x13 && s->mode!=9)cmd=3;
            break;
        case 7:s->event=0;s->menu_last=0x19;break;
        default:break;
        }
    }else if(s->view==2){
        uint8_t *p=s->record_selector;
        switch(event){
        case 3:cmd=pick(primary,cycle(p,5),0);p[1]=p[2]=0;s->event=0;break;
        case 4:cmd=pick(current,cycle(p+1,4),0x15);p[0]=p[2]=0;s->event=0;break;
        case 1:cmd=pick(other,cycle(p+2,2),0x0f);p[0]=p[1]=0;s->event=0;break;
        case 2:cmd=3;s->event=0;break;
        case 5:s->event=0;effect(io,DMT_EFFECT_HISTORY_APPEND,s);break;
        default:break;
        }
    }else{
        switch(event){
        case 3:cmd=s->monitor_selector?0x0d:0x0c;s->monitor_selector=!s->monitor_selector;s->event=0;break;
        case 4:cmd=s->mode==0x10?0x15:0x10;s->event=0;break;
        case 2:cmd=3;s->event=0;break;
        case 1:cmd=0x12;s->event=0;break;
        default:break;
        }
    }
    if(event){
        if(cmd){packet[3]=cmd;packet[9]=(uint8_t)(cmd+5);}
        if(io->send)io->send(io->context,packet);
    }
    if(s->view==1 && s->relative==1 && (s->relative_mode!=s->mode || s->relative_unit!=s->unit)){
        s->relative=0;effect(io,DMT_EFFECT_RELATIVE_OFF,s);
    }
}
