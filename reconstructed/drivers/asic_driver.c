#include "asic_driver.h"
enum dmt_asic_tx_kind asic_classify_tx(const uint8_t *p,size_t n)
{
    if(!p || n!=10 || p[0]!=0xaa || p[1]!=0x55)return DMT_ASIC_TX_INVALID;
    for(unsigned i=4;i<9;i++)if(p[i])return DMT_ASIC_TX_INVALID;
    if(p[2]==2 && p[3]==0xa0 && p[9]==0xa2)return DMT_ASIC_TX_DIAGNOSTIC_A0;
    if(p[2]!=5)return DMT_ASIC_TX_INVALID;
    if(!p[3] && p[9]==0x11)return DMT_ASIC_TX_LEGACY_ZERO;
    uint8_t expected[10];
    if(!measurement_build_command(p[3],expected) || p[9]!=expected[9])return DMT_ASIC_TX_INVALID;
    return DMT_ASIC_TX_COMMAND;
}
bool asic_transmit_observed(const dmt_asic_io *io,const uint8_t *p,size_t n)
{
    return io && io->transmit && asic_classify_tx(p,n)!=DMT_ASIC_TX_INVALID && io->transmit(io->context,p);
}
static bool command(const dmt_asic_io *io,uint8_t id)
{uint8_t p[10];return measurement_build_command(id,p) && asic_transmit_observed(io,p,10);}
bool asic_request_mode(const dmt_asic_io *io,uint8_t mode)
{return mode>=9 && mode<=0x16 && command(io,mode);}
bool asic_request_hold(const dmt_asic_io *io){return command(io,3);}
bool asic_forward_key(const dmt_asic_io *io,uint8_t key)
{static const uint8_t ids[]={9,8,3,1};return key>=1 && key<=4 && command(io,ids[key-1]);}
bool asic_replay_diagnostic_entry(const dmt_asic_io *io)
{
    /* This is a bounded replay of two literals, not a general A0 constructor. */
    static const uint8_t a0[]={0xaa,0x55,2,0xa0,0,0,0,0,0,0xa2};
    if(!io || !io->delay_ms || !command(io,0x0b))return false;
    io->delay_ms(io->context,200);
    return asic_transmit_observed(io,a0,sizeof a0);
}
measurement_frame_kind asic_receive_idle(const uint8_t *p,size_t n,measurement_frame *frame)
{return measurement_decode_frame(p,n,frame);}
bool asic_reading_update(measurement_reading *reading,const uint8_t *p,size_t n)
{return reading && p && n==13 && measurement_parse_reading(reading,p);}
