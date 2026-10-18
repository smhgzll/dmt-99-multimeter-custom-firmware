/* Test-only access to the real application controller. No second policy model.
 * Original base/history/reading callees are qualified independently and isolated
 * at the same call boundary in the instruction oracle and this trace. */
#include "application.h"
typedef void (*Trace)(void *,uint32_t,const uint32_t[8],const char *);
typedef struct {Trace trace;void *ctx;bool longpress,quiet_base;dmt_application *app;} Probe;
static void record(void *ctx,uint32_t pc,const uint32_t args[8],const char *text)
{
    Probe *p=ctx;if(p->quiet_base)return;
    p->trace(p->ctx,pc,args,text);if(pc==0x14ef2)p->quiet_base=true;
}
static void tx(void *ctx,const uint8_t packet[10])
{Probe *p=ctx;uint32_t args[8]={0};p->trace(p->ctx,0x97c4,args,(const char *)packet);}
static bool longpress(void *ctx,uint8_t event)
{Probe *p=ctx;uint32_t args[8]={event};p->trace(p->ctx,0x7b08,args,0);return p->longpress;}
static void delay(void *ctx,uint32_t ms)
{Probe *p=ctx;uint32_t args[8]={ms};p->trace(p->ctx,0x1a64c,args,0);}
static void buzzer(void *ctx,bool on)
{Probe *p=ctx;uint32_t args[8]={0};p->trace(p->ctx,on?0x78ba:0x78d0,args,0);}
void probe_buttons(dmt_control *control,uint16_t word,bool is_long,uint8_t menu_view,Trace trace,void *ctx,float extrema[2])
{
    dmt_application a={0};dmt_screen screen={0};Probe p={trace,ctx,is_long,false,&a};
    a.control=*control;a.run=DMT_RUNNING;a.rx_suppressed=1;a.event_consumed=1;
    a.menu=(dmt_menu){1,control->menu_last,1,false};a.history_row=1;*settings_image_word(&a.settings)=word;
    screen.reading.maximum=extrema[0];screen.reading.minimum=extrema[1];screen.view=(dmt_view){.context=&p,.trace=record};
    dmt_application_io io={.context=&p,.send=tx,.longpress=longpress,.delay_ms=delay,.buzzer=buzzer,.reading=&screen.reading,.view=&screen.view,.screen=&screen};
    application_step(&a,&io);
    if(a.run==DMT_MENU){a.control.view=menu_view;a.control.event=6;application_step(&a,&io);}
    *control=a.control;extrema[0]=screen.reading.maximum;extrema[1]=screen.reading.minimum;
}
