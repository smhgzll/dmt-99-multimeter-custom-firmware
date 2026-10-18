#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "measurement_asic.h"
#include "spi_flash.h"
#include "display_bus.h"
#include "buttons.h"
#include "history.h"
#include "foreground.h"

static unsigned passed;
#define RUN(fn) do{fn();++passed;printf("PASS %s\n",#fn);}while(0)
static void command_vectors(void){
    uint8_t out[10],expected[]={0xaa,0x55,5,0x0c,0,0,0,0,0,0x11};
    assert(measurement_build_command(0x0c,out));assert(!memcmp(out,expected,10));
    assert(measurement_build_command(9,out)&&out[9]==14);
    assert(!measurement_build_command(2,out));assert(!measurement_build_command(0xff,out));
}
static void segment_vectors(void){
    assert(measurement_segment_decode(0xeb)==0);assert(measurement_segment_decode(0xff)==8);
    assert(measurement_segment_decode(0x0a)==1);assert(measurement_segment_decode(0x24)==21);
    assert(measurement_segment_decode(0x11)==0xff);
}
static void packed_frame(void){
    uint8_t b[13]={0x5a,0xa5,0x10,0xba,0x8d,0x4f,0x0e};measurement_frame f;
    assert(measurement_decode_frame(b,13,&f)==MEASUREMENT_DISPLAY_FRAME);
    assert(!memcmp(f.symbols,"1234",4));assert(f.negative&&f.decimal_after==1&&f.unit_code==1);
    b[7]=0x81;b[8]=2;assert(measurement_decode_frame(b,13,&f)==MEASUREMENT_DISPLAY_FRAME);
    assert(f.unit_code==7&&f.high_status_bit);b[0]=0;assert(measurement_decode_frame(b,13,&f)==MEASUREMENT_INVALID);
    assert(measurement_decode_frame(b,12,&f)==MEASUREMENT_INVALID);
}
static void idle_capture(void){
    uint8_t b[13]={0x5a,0xa5};measurement_rx_state state={0};measurement_frame f;
    assert(measurement_frame_is_blank(b));measurement_capture_idle(&state,b);assert(state.suppress_measurement&&!state.event_pending);
    b[2]=0xfd;b[3]=0x0c;measurement_capture_idle(&state,b);
    assert(!state.suppress_measurement&&state.event_pending&&!memcmp(state.saved_event,b,13));
    assert(measurement_decode_frame(b,13,&f)==MEASUREMENT_EVENT_FRAME&&f.event==0x0c);
}
typedef struct{uint8_t tx[64],rx[64];unsigned count;int selects[8];unsigned nselect;}spi_mock;
static void select_mock(void *p,bool on){spi_mock *m=p;m->selects[m->nselect++]=on;}
static uint8_t transfer_mock(void *p,uint8_t v){spi_mock*m=p;m->tx[m->count]=v;return m->rx[m->count++];}
static void flash_wire_read(void){
    spi_mock m={0};dmt_spi_bus b={&m,select_mock,transfer_mock};uint8_t out[2];
    m.rx[4]=0x12;m.rx[5]=0x34;spi_flash_read(&b,out,0x123456,2);
    uint8_t expected[]={3,0x12,0x34,0x56,0xff,0xff};assert(m.count==6&&!memcmp(m.tx,expected,6));
    assert(out[0]==0x12&&out[1]==0x34&&m.selects[0]&&!m.selects[1]);
}
static void flash_id_and_pages(void){
    spi_mock m={0};dmt_spi_bus b={&m,select_mock,transfer_mock};
    m.rx[1]=0xef;m.rx[2]=0x40;m.rx[3]=0x17;assert(spi_flash_read_jedec_id(&b)==0xef4017);
    assert(m.tx[0]==0x9f);assert(spi_flash_page_chunk(0xff,100)==1);
    assert(spi_flash_page_chunk(0x100,300)==256);assert(spi_flash_page_chunk(0x100,12)==12);
}
typedef struct{unsigned code[32];uint16_t value[32];unsigned n;}display_mock;
static void pc_mock(void*p,uint16_t mask,bool hi){display_mock*m=p;m->code[m->n]=hi?1:0;m->value[m->n++]=mask;}
static void pb_mock(void*p,uint16_t value){display_mock*m=p;m->code[m->n]=2;m->value[m->n++]=value;}
static void display_strobes(void){
    display_mock m={0};dmt_display_bus b={&m,pc_mock,pb_mock};display_write_command(&b,0x2a);
    assert(m.n==6&&m.value[0]==0x20&&m.code[0]==0&&m.code[2]==2&&m.value[2]==0x2a);
    assert(m.value[3]==0x2000&&!m.code[3]&&m.code[4]&&m.value[5]==0x10&&m.code[5]);
    display_write_data(&b,0xf800);assert(m.code[6]==1&&m.value[8]==0xf800);
}
typedef struct{unsigned elapsed,reads,release_after,clear_count,power_downs;uint16_t high_mask;}button_mock;
static bool pin_mock(void*p,uint16_t mask){button_mock*m=p;++m->reads;return (m->high_mask&mask)||(m->release_after&&m->reads>m->release_after);}
static void delay_mock(void*p,uint16_t n){((button_mock*)p)->elapsed+=n;}
static void clear_mock(void*p,uint16_t mask){(void)mask;((button_mock*)p)->clear_count++;}
static void power_mock(void*p){((button_mock*)p)->power_downs++;}
static dmt_button_io button_io(button_mock*m){return(dmt_button_io){m,pin_mock,delay_mock,clear_mock,power_mock};}
static void button_long_press(void){
    button_mock m={0};dmt_button_io io=button_io(&m);uint8_t event=0;uint16_t idle=999;
    buttons_exti_9_5(&io,0x100,&event,&idle);assert(event==6&&m.elapsed==630&&idle==0&&m.clear_count==2);
}
static void button_short_and_priority(void){
    button_mock m={.release_after=3};dmt_button_io io=button_io(&m);uint8_t event=0;uint16_t idle=1;
    buttons_exti_9_5(&io,0x100,&event,&idle);assert(event==5&&m.elapsed==40);
    m=(button_mock){0};buttons_exti_15_10(&io,0x1c00,&event,&idle);assert(event==4&&m.elapsed==60&&m.clear_count==3);
    buttons_exti_2(&io,&event,true);assert(event==8&&m.power_downs==1);
}
static void history_roundtrip(void){
    const float inputs[]={1.25f,-9.5f,10.0f,12.5f,100.0f,125.0f,1000.0f,1250.0f};
    for(unsigned i=0;i<sizeof inputs/sizeof*inputs;++i){dmt_history_record r;assert(history_encode(12,7,inputs[i],0,&r));assert(history_decode(r)==inputs[i]);assert((r.metadata&63)==12&&((r.metadata>>6)&63)==7);}
    dmt_history_record r;assert(history_encode(12,1,0,1,&r)&&r.magnitude==65535);assert(history_encode(12,1,0,2,&r)&&r.magnitude==61166);
    assert(!history_encode(12,1,INFINITY,0,&r));assert(!history_encode(12,1,NAN,0,&r));
}
static void history_capacity(void){
    dmt_history h={0};for(unsigned i=0;i<35;++i)history_push(&h,(dmt_history_record){12,(uint16_t)i});
    assert(sizeof(dmt_history_record)==4&&h.count==30&&h.records[0].magnitude==34&&h.records[29].magnitude==5);
}
typedef struct{char trace[32];unsigned n;}foreground_mock;
static void mark(void*p,char c){foreground_mock*m=p;m->trace[m->n++]=c;}
static void regular_buttons(void*p){mark(p,'A');}static void record_buttons(void*p){mark(p,'B');}static void monitor_buttons(void*p){mark(p,'C');}
static void parse_measurement(void*p,const uint8_t*b){assert(b[0]==0x5a);mark(p,'P');}
static void regular_event(void*p){mark(p,'U');}static void record_event(void*p){mark(p,'V');}static void monitor_event(void*p){mark(p,'W');}
static void charge_poll(void*p){mark(p,'Q');}static uint8_t battery_read(void*p){mark(p,'R');return 42;}
static void battery_0(void*p){mark(p,'S');}static void battery_1(void*p){mark(p,'T');}
static dmt_foreground_ops foreground_ops(foreground_mock*m){return(dmt_foreground_ops){m,regular_buttons,record_buttons,monitor_buttons,parse_measurement,regular_event,record_event,monitor_event,charge_poll,battery_read,battery_0,battery_1};}
static void foreground_record_sequence(void){
    foreground_mock m={0};dmt_foreground_ops o=foreground_ops(&m);
    dmt_foreground_state s={.view=2,.mode=12,.battery_due=1};
    for(unsigned i=0;i<16;++i){s.saved_event[i]=(uint8_t)i;}
    s.saved_event[0]=0x5a;
    foreground_iteration(&s,&o);
    assert(!strcmp(m.trace,"BPVQRS")&&s.battery_percent==42&&!s.battery_due);
    assert(s.event_consumed&&s.rx_suppressed&&s.last_ui_event==0xff&&!memcmp(s.frame,s.saved_event,16));
}
static void foreground_hold_and_suppression(void){
    foreground_mock m={0};dmt_foreground_ops o=foreground_ops(&m);
    dmt_foreground_state s={.view=3,.hold=1,.event_consumed=1,.mode=0x13,.battery_due=1};
    foreground_iteration(&s,&o);assert(!strcmp(m.trace,"CWQ")&&!s.battery_due);
    m=(foreground_mock){0};s.view=1;s.layout=1;s.mode=12;s.battery_due=1;
    foreground_iteration(&s,&o);assert(!strcmp(m.trace,"AQRT"));
}
int main(void){
    RUN(command_vectors);RUN(segment_vectors);RUN(packed_frame);RUN(idle_capture);
    RUN(flash_wire_read);RUN(flash_id_and_pages);RUN(display_strobes);
    RUN(button_long_press);RUN(button_short_and_priority);RUN(history_roundtrip);RUN(history_capacity);
    RUN(foreground_record_sequence);RUN(foreground_hold_and_suppression);
    printf("%u tests passed\n",passed);return 0;
}
