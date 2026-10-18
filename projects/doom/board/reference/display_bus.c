/* 0x7d6a/0x7dba: PB0..15 data, PC5 D/C, PC4 select, PC13 write strobe. */
#include "display_bus.h"
static void write_word(const dmt_display_bus *b,uint16_t value,bool data)
{
    b->set_pc(b->context,0x20,data);
    b->set_pc(b->context,0x10,false);
    b->write_pb(b->context,value);
    b->set_pc(b->context,0x2000,false);
    b->set_pc(b->context,0x2000,true);
    b->set_pc(b->context,0x10,true);
}
void display_write_command(const dmt_display_bus *b,uint16_t value){write_word(b,value,false);}
void display_write_data(const dmt_display_bus *b,uint16_t value){write_word(b,value,true);}
