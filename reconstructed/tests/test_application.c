#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "application.h"
#include "disk.h"
#include "format.h"
#include "status_view.h"
#include "foreground.h"
#include <setjmp.h>
typedef struct {unsigned sends,delays,gaps,buzzer_calls,shutdown,programs,erases;bool charge,buzzer;uint8_t battery,packet[10];uint32_t fail_at;} Fake;
static void tx(void *v,const uint8_t p[10]){Fake *f=v;++f->sends;memcpy(f->packet,p,10);}
static bool charge(void *v){return ((Fake *)v)->charge;}
static uint8_t battery(void *v){return ((Fake *)v)->battery;}
static void delay(void *v,uint32_t n){((Fake *)v)->delays+=n;}
static void buzzer(void *v,bool on){Fake *f=v;f->buzzer=on;++f->buzzer_calls;}
static bool gap(void *v,uint32_t address,const dmt_application *a){(void)address;(void)a;++((Fake *)v)->gaps;return false;}
static void latch(void *v,bool on){if(!on)++((Fake *)v)->shutdown;}
static bool begin(void *v){(void)v;return true;}
static bool erase(void *v,uint32_t a){assert(a==DMT_SETTINGS_ADDRESS);++((Fake *)v)->erases;return true;}
static uint32_t program(void *v,uint32_t a,uint16_t value){Fake *f=v;(void)value;assert(a==DMT_SETTINGS_ADDRESS+2*f->programs);++f->programs;return f->fail_at==f->programs?3:4;}
static void end(void *v){(void)v;}
static dmt_application_io io_for(Fake *f){return (dmt_application_io){.context=f,.send=tx,.delay_ms=delay,.charging=charge,.battery_read=battery,.buzzer=buzzer,.power_latch=latch,.unresolved=gap};}
static dmt_settings_image image(unsigned view){dmt_settings_image s;settings_image_defaults(&s);s.halfwords[0]=0;settings_access(settings_image_word(&s),SETTINGS_THEME,1,view);return s;}
static void transitions(void)
{
    Fake f={.battery=80};dmt_application_io io=io_for(&f);dmt_settings_image s=image(1);dmt_application a;application_init(&a,&s,&io);
    assert(a.run==DMT_RUNNING && a.control.event==5 && a.control.mode==0x14);application_step(&a,&io);
    assert(f.sends==1 && f.packet[3]==0x14);
    uint8_t event[13]={0x5a,0xa5,0xfd,0x0c};application_receive_idle(&a,event);application_step(&a,&io);assert(a.control.mode==0x0c);
    event[3]=3;application_receive_idle(&a,event);application_step(&a,&io);assert(a.control.hold==1);
    event[3]=0x0d;application_receive_idle(&a,event);application_step(&a,&io);assert(a.control.hold==0 && a.control.mode==0x0d);
    a.control.event=6;application_step(&a,&io);assert(a.run==DMT_MENU);
    a.menu.state=6;a.control.event=5;application_step(&a,&io);assert(a.run==DMT_EDIT_THRESHOLD && a.control.event==0);
    a.control.event=6;application_step(&a,&io);assert(a.run==DMT_MENU && a.control.event==6);
    application_step(&a,&io);assert(a.run==DMT_RUNNING && a.control.event==5);
}
static void settings_and_charging(void)
{
    Fake f={.battery=50};dmt_application_io io=io_for(&f);dmt_settings_image s=image(2);dmt_application a;
    s.halfwords[0]=0xffff;application_init(&a,&s,&io);assert(a.run==DMT_LANGUAGE_SELECT);
    a.control.event=3;application_step(&a,&io);assert(a.language_choice==2);
    a.control.event=5;application_step(&a,&io);assert(a.run==DMT_RUNNING && (*settings_image_word(&a.settings)&7)==1);
    f.charge=true;application_step(&a,&io);assert(a.run==DMT_CHARGING);
    f.charge=false;application_step(&a,&io);assert(a.run==DMT_RUNNING);
    /* Unbound settings dependencies are visible failures. */
    application_init(&a,0,&io);assert(a.run==DMT_STOPPED && a.last_unresolved_address==0xd928);
}
static void history_integration(void)
{
    Fake f={.battery=50};dmt_application_io io=io_for(&f);dmt_settings_image s=image(2);dmt_application a;application_init(&a,&s,&io);
    a.control.event=0;a.control.value=-1.234f;a.control.unit=6;a.control.mode=0x0c;memcpy(a.control.text,"-1.234",7);
    for(unsigned i=0;i<35;++i){a.control.event=5;application_step(&a,&io);}
    assert(a.history.count==30 && a.settings.halfwords[16]==30 && a.history_row==4);
    assert(a.history.records[0].metadata==a.settings.halfwords[17]);
    assert(history_decode(a.history.records[0])<-1.23f);
    assert(f.packet[3]==0 && f.packet[9]==0x11);
}
static void power_and_persistence(void)
{
    Fake f={.battery=50};dmt_application_io io=io_for(&f);dmt_settings_image s=image(1);dmt_application a;
    dmt_settings_backend backend={&f,0,begin,erase,program,end};io.internal_flash=&backend;
    application_init(&a,&s,&io);settings_auto_off_set(settings_image_word(&a.settings),1);a.power.threshold_seconds=20;
    for(unsigned i=0;i<21;++i)application_timer(&a,&io);
    assert(a.power.idle_seconds==20 && a.run==DMT_RUNNING && a.power.battery_due);
    for(unsigned i=0;i<21;++i)application_timer(&a,&io);
    assert(a.run==DMT_STOPPED && f.programs==128 && f.erases==1 && f.shutdown==1);
    f.programs=0;f.fail_at=3;assert(settings_image_commit(&s,&backend,4)==3 && f.programs==3);
    f.programs=0;assert(settings_image_commit(&s,&backend,3)==3 && f.programs==0);
}
static int negative_level(void *v){(void)v;return -16;}
static void low_battery_signed_level(void)
{
    /* 7780 compares the signed 75f0 result: below 3400 mV (negative percent)
     * shuts down unless PA8 reports charging. */
    Fake f={.battery=50};dmt_application_io io=io_for(&f);dmt_settings_image s=image(1);dmt_application a;
    io.battery_level=negative_level;f.charge=true;application_init(&a,&s,&io);
    for(unsigned i=0;i<21;++i)application_timer(&a,&io);
    assert(a.run==DMT_RUNNING && a.power.battery_due && !f.shutdown);
    f.charge=false;a.power.battery_due=0;
    for(unsigned i=0;i<21;++i)application_timer(&a,&io);
    assert(a.run==DMT_STOPPED && a.power.shutdown && f.shutdown==1 && !a.power.battery_due);
}
typedef struct {uint8_t bytes[8192],command;unsigned phase,address,index,erases,pages;bool selected,enabled;} Nor;
static void select_nor(void *v,bool on)
{
    Nor *n=v;
    if(on){n->selected=true;n->phase=0;n->index=0;return;}
    if(n->command==6)n->enabled=true;
    if(n->command==0x20){assert(n->enabled);memset(n->bytes+(n->address&~4095u),255,4096);++n->erases;n->enabled=false;}
    if(n->command==2){++n->pages;n->enabled=false;}
    n->selected=false;
}
static uint8_t transfer_nor(void *v,uint8_t b)
{
    Nor *n=v;assert(n->selected);
    if(n->phase++==0){n->command=b;n->address=0;return 255;}
    if(n->command==5)return 0;
    if(n->phase<=4 && (n->command==2 || n->command==3 || n->command==0x20)){n->address=(n->address<<8)|b;return 255;}
    if(n->command==3){assert(n->address+n->index<8192);return n->bytes[n->address+n->index++];}
    if(n->command==2){assert(n->enabled);unsigned a=(n->address&~255u)|((n->address+n->index++)&255u);assert(a<8192);n->bytes[a]&=b;}
    return 255;
}
static void nor_sector_preservation(void)
{
    Nor n={0};uint8_t expected[8192],scratch[4096],source[300];
    for(unsigned i=0;i<8192;++i)n.bytes[i]=expected[i]=(uint8_t)(i*13);
    for(unsigned i=0;i<300;++i)source[i]=(uint8_t)(i*7);
    memcpy(expected+4090,source,300);dmt_spi_bus bus={&n,select_nor,transfer_nor};
    assert(spi_flash_write_preserve_sector(&bus,source,4090,300,scratch,10));
    assert(memcmp(n.bytes,expected,8192)==0 && n.erases==2 && n.pages==32);
    memset(n.bytes,255,sizeof n.bytes);n.erases=n.pages=0;
    assert(spi_flash_write_preserve_sector(&bus,source,250,300,scratch,10));
    assert(n.erases==0 && n.pages==3 && memcmp(n.bytes+250,source,300)==0);
    uint32_t sectors=0;uint16_t size=0;assert(disk_ioctl_recovered(1,1,&sectors)==0 && sectors==2048);
    assert(disk_ioctl_recovered(0,2,&size)==0 && size==4096);assert(disk_ioctl_recovered(2,1,&sectors)==4);
}
static void formatting(void)
{char b[32];assert(dmt_format(b,sizeof b,"%s/%d:%02x","res",12,10)==10 && !strcmp(b,"res/12:0a"));measurement_format_value(b,sizeof b,12.5f,0x12,21);assert(!strcmp(b," 0012"));assert(dmt_format(b,3,"abcd")==0 && b[2]==0);}
static void scheduler_slots(void)
{
 Fake f={.battery=80};dmt_application_io io=io_for(&f);dmt_settings_image s=image(1);dmt_application a;application_init(&a,&s,&io);
 settings_auto_off_set(settings_image_word(&a.settings),1);a.power.threshold_seconds=300;
 a.control.event=6;application_step(&a,&io);assert(a.run==DMT_MENU);
 uint8_t e[13]={0x5a,0xa5,0xfd,0x0c};application_receive_idle(&a,e);
 e[3]=0x0d;application_receive_idle(&a,e); /* latest event replaces previous */
 uint8_t old=a.control.mode;application_step(&a,&io);assert(a.control.mode==old && !a.event_consumed);
 for(unsigned i=0;i<21;i++)application_timer(&a,&io);
 assert(a.run==DMT_MENU && a.power.idle_seconds==20 && !a.event_consumed);
 a.control.event=6;application_step(&a,&io);assert(a.run==DMT_RUNNING && !a.event_consumed);
 application_step(&a,&io);assert(a.event_consumed && a.control.mode==0x0d);
}
static jmp_buf reference_exit;
static dmt_foreground_state reference_state;
static unsigned reference_polls,reference_parses,reference_entry;
static bool reference_diagnostic;
static bool reference_pin(void *ctx,uint16_t pin){(void)ctx;(void)pin;return !reference_diagnostic;}
static void reference_pwm(void *ctx,uint16_t period,uint16_t prescaler,uint16_t pulse)
{(void)ctx;assert(period==100 && prescaler==4799 && !pulse);}
static void reference_delay(void *ctx,uint32_t ms){(void)ctx;assert(ms==1000);}
static void reference_begin(void *ctx){(void)ctx;reference_entry++;}
static void reference_noop(void *ctx){(void)ctx;}
static void reference_poll(void *ctx)
{(void)ctx;if(++reference_polls==3)longjmp(reference_exit,1);}
static void reference_parse(void *ctx,const uint8_t *frame){(void)ctx;(void)frame;reference_parses++;}
static const char *reference_text(void *ctx){(void)ctx;return " 12.34";}
static void reference_draw(void *ctx,uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint8_t size,uint16_t color,const char *text)
{(void)ctx;(void)text;assert(x==100 && y==100 && w==100 && h==50 && size==32 && color==0xfd40);}
static void reference_unit(void *ctx,uint16_t x,uint16_t y){(void)ctx;assert(x==200 && y==110);}
static void complete_loop_reference(void)
{
 for(volatile unsigned diagnostic=0;diagnostic<2;diagnostic++){
  reference_diagnostic=diagnostic;reference_polls=reference_parses=reference_entry=0;
  reference_state=(dmt_foreground_state){.view=1,.rx_suppressed=diagnostic?0:1,.event_consumed=0};
  uint8_t event=0,normal=0;
  dmt_foreground_entry_ops enter={0,reference_pin,reference_pwm,reference_delay,reference_begin,reference_noop,reference_noop};
  dmt_foreground_ops run={.regular_buttons=reference_poll,.charge_screen_poll=reference_noop};
  dmt_diagnostic_ops diag={0,reference_poll,reference_parse,reference_text,reference_draw,reference_unit};
  if(!setjmp(reference_exit))foreground_loop(&reference_state,&event,&normal,&enter,&run,&diag);
  assert(reference_polls==3 && reference_entry==diagnostic);
  if(diagnostic)assert(reference_parses==1 && reference_state.event_consumed==1 && reference_state.rx_suppressed==0);
  else assert(!reference_parses && reference_state.mode==20);
 }
}
int main(void)
{
    transitions();settings_and_charging();history_integration();power_and_persistence();low_battery_signed_level();nor_sector_preservation();formatting();scheduler_slots();complete_loop_reference();
    puts("9 application/storage integration tests passed");return 0;
}
