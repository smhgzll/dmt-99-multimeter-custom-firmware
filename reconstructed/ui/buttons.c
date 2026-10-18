/* 0x7c0a,0x7c42,0x7cd8. Deliberately preserve blocking debounce and event overwrite order. */
#include "buttons.h"
void buttons_exti_2(const dmt_button_io *io,uint8_t *event,bool normal)
{
    io->delay_ms(io->context,20);
    if(!io->pin_high(io->context,4) && normal)io->power_down(io->context);
    *event=BUTTON_PC2;io->clear_exti(io->context,4);
}
void buttons_exti_9_5(const dmt_button_io *io,uint16_t pending,uint8_t *event,uint16_t *idle)
{
    if(pending&0x200){io->delay_ms(io->context,20);if(!io->pin_high(io->context,0x200))*event=BUTTON_PC9;}
    if(pending&0x100){
        io->delay_ms(io->context,20);
        if(!io->pin_high(io->context,0x100)){
            *event=BUTTON_PC8_SHORT;
            unsigned count=61;
            while(!io->pin_high(io->context,0x100)){
                --count;io->delay_ms(io->context,10);
                if(count==0){*event=BUTTON_PC8_LONG;break;}
            }
        }
    }
    *idle=0;io->clear_exti(io->context,0x200);io->clear_exti(io->context,0x100);
}
void buttons_exti_15_10(const dmt_button_io *io,uint16_t pending,uint8_t *event,uint16_t *idle)
{
    const uint16_t pins[]={0x400,0x800,0x1000};
    const uint8_t events[]={BUTTON_PC10,BUTTON_PC11,BUTTON_PC12};
    for(unsigned i=0;i<3;++i)if(pending&pins[i]){
        io->delay_ms(io->context,20);if(!io->pin_high(io->context,pins[i]))*event=events[i];
    }
    *idle=0;
    for(unsigned i=0;i<3;++i)io->clear_exti(io->context,pins[i]);
}

/* 0x7b08: five polling keys, 41..101 low samples count as a long press. */
bool buttons_wait_longpress(const dmt_button_io *io,uint8_t key,uint8_t *event)
{
    static const uint16_t pins[]={0x400,0x800,0x200,0x1000,0x100};
    if(key<1 || key>5)return false;
    unsigned count=0;
    while(!io->pin_high(io->context,pins[key-1])){
        if(count==101){*event=0;return true;}
        ++count;io->delay_ms(io->context,10);
    }
    if(count<41)return false;
    *event=0;return true;
}
