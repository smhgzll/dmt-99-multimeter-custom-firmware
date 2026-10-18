#include "simulator.h"
#include "config.h"
#include "gauge.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static dmt_simulator sim;
static unsigned menu_clears,menu_draws;
static void menu_trace(void *p,uint32_t pc,const uint32_t args[8],const char *text)
{(void)p;(void)args;(void)text;menu_draws++;menu_clears+=pc==0x876a;}
static void menu_rendering(void)
{
 for(unsigned layout=0;layout<2;layout++)for(unsigned language=0;language<2;language++){
  assert(simulator_configure(&sim,1,layout,language));
  sim.screen.view.trace=menu_trace;menu_clears=menu_draws=0;
  assert(simulator_button(&sim,6,false));assert(sim.app.menu.last==1 && menu_clears==1);
  unsigned draws=menu_draws;application_step(&sim.app,&sim.io);
  assert(menu_draws==draws); /* A stable menu does no resource/pixel work. */
  sim.pixels[225*320+300]=0x1234;
  assert(simulator_button(&sim,3,false));assert(sim.app.menu.state==2 && menu_clears==1);
  assert(sim.pixels[225*320+300]==0x1234); /* Cursor update preserves other regions. */
  assert(sim.pixels[52*320+156]==(layout?0xfd40:0x1c9f));
  assert(simulator_button(&sim,4,false));assert(sim.app.menu.state==1 && sim.app.control.event==4);
  draws=menu_draws;application_step(&sim.app,&sim.io);assert(menu_draws==draws && sim.app.control.event==4);
  /* View -> About -> Reset -> Settings; ENTER selects Theme1. */
  assert(simulator_button(&sim,2,false));assert(simulator_button(&sim,2,false));assert(simulator_button(&sim,2,false));
  assert(sim.app.menu.state==13);assert(simulator_button(&sim,3,false));assert(sim.app.menu.state==20);
  assert(simulator_button(&sim,1,false));assert(sim.app.menu.state==21);
  assert(simulator_button(&sim,5,false));assert(sim.app.control.layout==1);
  assert(menu_clears==(layout?1u:2u));
  assert(simulator_button(&sim,4,false));assert(sim.app.menu.state==13 && sim.app.menu.page==1);
  assert(simulator_button(&sim,6,false));assert(sim.app.run==DMT_RUNNING);
  assert(sim.screen.unit_cache==25 && sim.screen.auto_cache==255 && sim.app.menu.last==25);
 }
 assert(simulator_configure(&sim,2,0,1));
 sim.screen.reading.maximum=777;sim.screen.reading.minimum=-777;
 assert(simulator_button(&sim,6,false));assert(simulator_button(&sim,6,false));
 assert(sim.screen.reading.maximum==-9999 && sim.screen.reading.minimum==9999);
 assert(simulator_configure(&sim,1,0,1));sim.app.control.monitor_state=1;sim.app.control.monitor_selector=1;
 assert(simulator_button(&sim,6,false));assert(simulator_button(&sim,6,false));
 assert(sim.app.control.monitor_state==1 && sim.app.control.monitor_selector==1);
 assert(simulator_configure(&sim,3,0,1));sim.app.control.monitor_state=1;sim.app.control.monitor_selector=1;
 assert(simulator_button(&sim,6,false));assert(simulator_button(&sim,6,false));
 assert(!sim.app.control.monitor_state && !sim.app.control.monitor_selector);
}
int main(int argc,char **argv)
{
 assert(argc==2);assert(simulator_open(&sim,argv[1]));simulator_power_on(&sim);
 for(unsigned i=0;i<14;i++){size_t n;const uint8_t *p=scene_bitmap(0x254c8+i*264,&n);assert(p && n==261 && p[2]==38);}
 assert(scene_data(0x26337,1) && !scene_data(0x26337,2));assert(!scene_data(0xffffffffu,1));
 assert(sim.power && sim.app.run==DMT_RUNNING && sim.tx[3]==20);
 simulator_ack(&sim,12);assert(simulator_reading(&sim,"12.34",6));
 assert(sim.app.control.value>12.339f && sim.app.control.value<12.341f);
 assert(sim.app.control.unit==6 && sim.resources.images_opened>5 && !sim.resource_errors);
 unsigned lit=0;for(unsigned i=0;i<320*240;i++)lit+=sim.pixels[i]!=0;assert(lit>5000);
 assert(simulator_snapshot(&sim,"build/simulator-test.png"));
 /* Invalid human input never injects a partial packet. */
 uint32_t value;memcpy(&value,&sim.app.control.value,4);
 assert(!simulator_reading(&sim,"1.2.34",6));assert(!simulator_reading(&sim,"123",6));assert(!simulator_reading(&sim,"12345",6));assert(!simulator_reading(&sim,"ABCD",6));assert(!simulator_reading(&sim,"1234",255));
 uint32_t after;memcpy(&after,&sim.app.control.value,4);assert(value==after);
 assert(!simulator_button(&sim,8,false));
 /* HOLD ACK changes host policy; frames received while held do not replace value. */
 assert(simulator_button(&sim,2,false) && sim.tx[3]==3);simulator_ack(&sim,3);assert(sim.app.control.hold);
 float held=sim.app.control.value;assert(simulator_reading(&sim,"99.99",6));assert(sim.app.control.value==held);
 simulator_ack(&sim,3);assert(!sim.app.control.hold);assert(simulator_reading(&sim,"12.34",6));
 assert(simulator_button(&sim,2,true));assert(sim.app.control.relative);
 assert(simulator_reading(&sim,"13.34",6));assert(sim.app.control.relative && sim.app.control.text[0]);
 assert(simulator_button(&sim,6,false) && sim.app.run==DMT_MENU);
 assert(simulator_button(&sim,3,false) && sim.app.menu.state==2);
 assert(simulator_button(&sim,1,false) && sim.app.menu.state==3);
 assert(simulator_button(&sim,5,false) && sim.app.control.view==2);
 assert(simulator_button(&sim,6,false) && sim.app.run==DMT_RUNNING);
 simulator_ack(&sim,12);assert(simulator_reading(&sim,"24.68",6));
 assert(simulator_button(&sim,5,false));assert(sim.app.history.count==1);
 application_power_down(&sim.app,&sim.io);assert(!sim.power && sim.settings_writes==128);
 simulator_power_on(&sim);assert(sim.app.history.count==1 && sim.app.control.view==2);
 /* Change mode, shut down and restore through explicit ACK. */
 simulator_ack(&sim,15);application_power_down(&sim.app,&sim.io);simulator_power_on(&sim);
#ifdef CUSTOM_DMT99
 assert(sim.tx[3]==15 && sim.app.control.mode==12);simulator_ack(&sim,15);assert(sim.app.control.mode==15 && sim.last_writes>0);
 assert(last_mode_forget(&sim.last_backend));assert(!last_mode_load(&sim.last_backend,2,&sim.tx[0]));
#else
 assert(sim.tx[3]==12 && sim.app.control.mode==12);assert(!sim.last_reads && !sim.last_writes);
#endif
 assert(simulator_state_save(&sim,"build/simulator-test.state"));assert(simulator_state_load(&sim,"build/simulator-test.state"));
 sim.charging=true;application_step(&sim.app,&sim.io);assert(sim.app.run==DMT_CHARGING);
 sim.charging=false;application_step(&sim.app,&sim.io);assert(sim.app.run==DMT_RUNNING);
 assert(simulator_configure(&sim,3,0,1));simulator_ack(&sim,12);
 assert(simulator_reading(&sim,"300.0",6));assert(sim.app.alarm==2 && sim.buzzer);
 assert(simulator_reading(&sim,"0010",6));assert(sim.app.alarm==0 && !sim.buzzer);
 sim.screen.history_unit_y=123;sim.app.history_row=8;sim.app.history_wrapped=1;
 application_reset_history_strip(&sim.app,&sim.io);assert(sim.app.history_row==1 && !sim.app.history_wrapped && !sim.screen.history_unit_y);
 application_invalidate_display(&sim.app,&sim.io);assert(sim.app.alarm==255 && sim.screen.unit_cache==25 && sim.screen.auto_cache==255);
 assert(simulator_configure(&sim,1,0,1));simulator_ack(&sim,19);assert(simulator_reading(&sim,"1234",6));
 assert(sim.screen.unit_cache==24);simulator_ack(&sim,12);assert(simulator_reading(&sim,"1234",6));assert(sim.screen.unit_cache==7);
 unsigned before=sim.resources.images_opened;simulator_ack(&sim,19);assert(simulator_reading(&sim,"1234",6));assert(sim.resources.images_opened>before && sim.screen.unit_cache==24);
 menu_rendering();
 sim.screen.auto_cache=4;simulator_ack(&sim,12);assert(sim.screen.auto_cache==255);
 assert(!sim.rejected_tx);
 unsigned tx_before=sim.tx_count,writes_before=sim.settings_writes,last_reads=sim.last_reads,last_writes=sim.last_writes;uint64_t elapsed=sim.elapsed_ms;
 assert(simulator_diagnostic_boot(&sim));assert(sim.app.run==DMT_DIAGNOSTIC && sim.tx_count==tx_before+2);
 assert(sim.tx[2]==2 && sim.tx[3]==0xa0 && sim.tx[9]==0xa2 && sim.elapsed_ms==elapsed+1200);
 assert(!sim.app.settings_loaded && sim.settings_writes==writes_before);
 assert(sim.last_reads==last_reads && sim.last_writes==last_writes);
 assert(simulator_button(&sim,3,false) && sim.tx[3]==3); /* diagnostic key forwarding, not normal mode cycle */
 uint8_t event[13]={0x5a,0xa5,0xfd,12};simulator_packet(&sim,event);
 assert(sim.app.event_consumed==1 && sim.app.rx_suppressed==0); /* event is skipped once */
 assert(simulator_reading(&sim,"12.34",6));assert(sim.app.control.value>12.33f && sim.app.control.value<12.35f);
 assert(sim.app.control.mode==0); /* diagnostic reception never enters the normal FD mode dispatcher */
 unsigned images=sim.resources.images_opened;application_step(&sim.app,&sim.io);
 assert(sim.app.rx_suppressed==0 && sim.resources.images_opened==images && !sim.rejected_tx);
 simulator_close(&sim);puts("7 simulator integration tests passed");return 0;
}
