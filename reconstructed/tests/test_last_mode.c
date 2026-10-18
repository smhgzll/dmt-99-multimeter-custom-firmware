#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "application.h"
typedef struct {uint8_t bytes[16],last[10];bool present,fail;unsigned reads,writes,sends;} Mock;
static bool read_record(void *p,uint8_t b[16]){Mock *m=p;m->reads++;if(!m->present)return false;memcpy(b,m->bytes,16);return true;}
static bool write_record(void *p,const uint8_t b[16]){Mock *m=p;if(m->fail)return false;memcpy(m->bytes,b,16);m->present=true;m->writes++;return true;}
static void tx(void *p,const uint8_t b[10]){Mock *m=p;memcpy(m->last,b,10);m->sends++;}
static dmt_settings_image image(void){dmt_settings_image x;settings_image_defaults(&x);x.halfwords[0]=0;settings_access(settings_image_word(&x),SETTINGS_THEME,1,1);return x;}
int main(void)
{
 Mock m={0};dmt_last_mode_backend backend={&m,read_record,write_record,0};dmt_settings_image x=image();dmt_application a;dmt_application_io io={.context=&m,.send=tx,.last_mode=&backend};
 application_init(&a,&x,&io);application_step(&a,&io);assert(m.last[3]==0x14);
 uint8_t b[13]={0x5a,0xa5,0xfd,0x0d};application_receive_idle(&a,b);application_step(&a,&io);assert(a.control.mode==0xd);
#ifdef CUSTOM_DMT99
 assert(m.present && m.writes==1);uint8_t record[16];memcpy(record,m.bytes,16);
 b[3]=3;application_receive_idle(&a,b);application_step(&a,&io);assert(m.writes==1);
 application_init(&a,&x,&io);assert(m.last[3]==0xd && m.last[9]==0x12 && a.control.event==0 && a.control.mode==0x14);
 b[3]=0xd;application_receive_idle(&a,b);application_step(&a,&io);assert(a.control.mode==0xd && m.writes==1);
 a.control.event=3;application_step(&a,&io);assert(m.last[3]==0xb); /* selector follows restoredDC/AC position */
 m.bytes[5]^=1;application_init(&a,&x,&io);assert(a.control.event==5);application_step(&a,&io);assert(m.last[3]==0x14);memcpy(m.bytes,record,16);
 uint8_t mode=0;assert(!last_mode_load(&backend,2,&mode));m.fail=true;assert(!last_mode_save(&backend,1,0x11));assert(!memcmp(record,m.bytes,16));
 assert(!last_mode_save(&backend,1,255));
 m.fail=false;a.run=DMT_MENU;a.menu.state=31;a.control.event=5;application_step(&a,&io);
 assert(!last_mode_load(&backend,1,&mode));
 puts("5 custom last-mode tests passed");
#else
 assert(!m.present && !m.reads && !m.writes);application_init(&a,&x,&io);application_step(&a,&io);assert(m.last[3]==0x14);
 puts("2 stock isolation tests passed");
#endif
 return 0;
}
