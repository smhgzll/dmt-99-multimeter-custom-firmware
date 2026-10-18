#include "asic_driver.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {uint8_t packets[8][10];unsigned sent,delay,fail_on;} Wire;
static bool tx(void *ctx,const uint8_t p[10])
{Wire *w=ctx;assert(w->sent<8);memcpy(w->packets[w->sent++],p,10);return w->sent!=w->fail_on;}
static void delay(void *ctx,uint32_t ms){((Wire *)ctx)->delay+=ms;}
static void requests(void)
{
 for(unsigned mode=0;mode<256;mode++){
  Wire w={0};dmt_asic_io io={&w,tx,delay};
  assert(asic_request_mode(&io,mode)==(mode>=9 && mode<=22));
  if(w.sent){assert(w.sent==1 && w.packets[0][2]==5 && w.packets[0][3]==mode && w.packets[0][9]==mode+5);}
 }
 Wire w={0};dmt_asic_io io={&w,tx,delay};assert(asic_request_hold(&io));assert(w.packets[0][3]==3);
 for(unsigned key=1;key<=4;key++)assert(asic_forward_key(&io,key));
 assert(!asic_forward_key(&io,0) && !asic_forward_key(&io,5));
 assert(w.packets[1][3]==9 && w.packets[2][3]==8 && w.packets[3][3]==3 && w.packets[4][3]==1);
 assert(!asic_request_mode(0,12));w.fail_on=w.sent+1;assert(!asic_request_mode(&io,12));
}
static void observed_packets(void)
{
 uint8_t packet[10]={0xaa,0x55,5,0,0,0,0,0,0,0x11};Wire w={0};dmt_asic_io io={&w,tx,delay};
 assert(asic_classify_tx(packet,10)==DMT_ASIC_TX_LEGACY_ZERO);assert(asic_transmit_observed(&io,packet,10));
 for(unsigned n=0;n<10;n++)assert(!asic_transmit_observed(&io,packet,n));
 assert(!asic_transmit_observed(&io,packet,11));assert(!asic_transmit_observed(&io,0,10));
 packet[9]=5;assert(asic_classify_tx(packet,10)==DMT_ASIC_TX_INVALID); /* no guessed additive zero frame */
 packet[3]=0xa0;packet[9]=0xa5;assert(asic_classify_tx(packet,10)==DMT_ASIC_TX_INVALID);
 packet[2]=2;packet[9]=0xa2;assert(asic_classify_tx(packet,10)==DMT_ASIC_TX_DIAGNOSTIC_A0);
 packet[4]=1;assert(asic_classify_tx(packet,10)==DMT_ASIC_TX_INVALID);
 assert(w.sent==1);
}
static void diagnostic(void)
{
 Wire w={0};dmt_asic_io io={&w,tx,delay};assert(asic_replay_diagnostic_entry(&io));
 static const uint8_t first[]={0xaa,0x55,5,0xb,0,0,0,0,0,0x10},second[]={0xaa,0x55,2,0xa0,0,0,0,0,0,0xa2};
 assert(w.sent==2 && w.delay==200 && !memcmp(first,w.packets[0],10) && !memcmp(second,w.packets[1],10));
 w=(Wire){.fail_on=1};assert(!asic_replay_diagnostic_entry(&io));assert(w.sent==1 && !w.delay);
 w=(Wire){.fail_on=2};assert(!asic_replay_diagnostic_entry(&io));assert(w.sent==2 && w.delay==200);
 io.delay_ms=0;w=(Wire){0};assert(!asic_replay_diagnostic_entry(&io) && !w.sent);
}
static void receive(void)
{
 /* Packed original segment codes for1234, decimal after2, volts. */
 uint8_t packet[13]={0x5a,0xa5,0x01,0xaa,0x9d,0x4f,0x0e,0,2,0x91,0x23,0x45,0x67};
 measurement_frame frame;assert(asic_receive_idle(packet,13,&frame)==MEASUREMENT_DISPLAY_FRAME);
 assert(!memcmp(frame.raw,packet,13) && !memcmp(frame.symbols,"1234",4));
 assert(frame.decimal_after==2 && frame.unit_code==6 && frame.ac_dc_code==1);
 measurement_reading reading={0};assert(asic_reading_update(&reading,packet,13));
 assert(!strcmp(reading.text," 12.34") && reading.numeric==1 && reading.value>12.339f && reading.value<12.341f);
 float saved=reading.value;assert(!asic_reading_update(&reading,packet,12) && reading.value==saved);
 for(unsigned n=0;n<13;n++)assert(asic_receive_idle(packet,n,&frame)==MEASUREMENT_INVALID);
 packet[0]=0;assert(asic_receive_idle(packet,13,&frame)==MEASUREMENT_INVALID);assert(!asic_reading_update(&reading,packet,13));
 packet[0]=0x5a;packet[2]=0xfd;packet[3]=3;
 assert(asic_receive_idle(packet,13,&frame)==MEASUREMENT_EVENT_FRAME && frame.event==3);
 /* Receipt does not assert a correlated ACK, change modes or synthesize replies. */
 assert(reading.mode==0 && frame.raw[9]==0x91 && frame.raw[12]==0x67);
}
int main(void){requests();observed_packets();diagnostic();receive();puts("4 standalone ASIC driver tests passed");return 0;}
