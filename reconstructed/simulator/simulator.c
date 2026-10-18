/* Host-only, deterministic execution of maintained C. UART ACKs, timers and
 * battery values are explicit inputs. This does not model ASIC internals or
 * interrupt cycle timing. The acquired SPI image is opened rb and never written. */
#include "simulator.h"
#include "png.h"
#include "history_view.h"
#include "monitor.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
static int flash_read(void *p,uint32_t at,void *out,uint32_t n)
{dmt_simulator *s=p;if(at>8388608 || n>8388608-at)return 1;memcpy(out,s->flash+at,n);return 0;}
static int flash_write(void *p,uint32_t at,const void *data,uint32_t n)
{dmt_simulator *s=p;if(at>8388608 || n>8388608-at)return 1;memcpy(s->flash+at,data,n);return 0;}
static void flash_gap(void *p,uint32_t address)
{(void)address;((dmt_simulator *)p)->external_gaps++;}
static void pixel(void *p,uint16_t x,uint16_t y,uint16_t c)
{dmt_simulator *s=p;if(x<320 && y<240)s->pixels[y*320+x]=c;else s->clipped_pixels++;}
#if DMT_THEMES
static void themed_pixel(void *p,uint16_t x,uint16_t y,uint16_t c)
{pixel(p,x,y,theme_color(&((dmt_simulator *)p)->theme,c));}
#endif
static void json_string(FILE *f,const char *text)
{
 fputc('"',f);
 for(const unsigned char *p=(const unsigned char *)text;p && *p;p++){
  if(*p=='"' || *p=='\\')fprintf(f,"\\%c",*p);else if(*p<32 || *p>126)fprintf(f,"\\u%04x",*p);else fputc(*p,f);
 }
 fputc('"',f);
}
static bool image(void *p,const char *path,uint16_t x,uint16_t y)
{
 dmt_simulator *s=p;
#if DMT_THEMES
 char themed[96];
 if(theme_path(&s->theme,path,themed,sizeof themed) && resource_draw_jpeg(&s->resources,themed,x,y)==0)path=themed;
 else
#endif
 (void)resource_draw_jpeg(&s->resources,path,x,y);
 bool fail=s->resources.file_result!=FR_OK && s->resources.prepare_result==JDR_PAR;
 if(fail || s->resources.prepare_result!=JDR_OK || s->resources.decode_result!=JDR_OK)s->resource_errors++;
 if(s->draw_trace){
  bool ok=s->resources.prepare_result==JDR_OK;
  fprintf(s->draw_trace,"{\"op\":\"jpeg\",\"path\":");json_string(s->draw_trace,path);
  fprintf(s->draw_trace,",\"x\":%u,\"y\":%u,\"w\":%u,\"h\":%u,\"open\":%d,\"prepare\":%d,\"decode\":%d}\n",x,y,
   ok?s->resources.decoder.width:0,ok?s->resources.decoder.height:0,(int)s->resources.file_result,(int)s->resources.prepare_result,(int)s->resources.decode_result);
 }
 return fail;
}
/* Semantic view calls (original addresses from view.c) as named draw ops. */
static void view_trace_draw(void *p,uint32_t address,const uint32_t a[8],const char *text)
{
 dmt_simulator *s=p;FILE *f=s->draw_trace;if(!f)return;
 switch(address){
 case 0x85cc:fprintf(f,"{\"op\":\"text_box\",\"x\":%u,\"y\":%u,\"w\":%u,\"h\":%u,\"size\":%u,\"color\":%u,\"text\":",a[0],a[1],a[2],a[3],a[4],a[5]);break;
 case 0x8588:fprintf(f,"{\"op\":\"ascii\",\"x\":%u,\"y\":%u,\"color\":%u,\"background\":%u,\"size\":%u,\"text\":",a[0],a[1],a[3],a[4],a[5]);break;
 case 0x8e46:fprintf(f,"{\"op\":\"chinese\",\"x\":%u,\"y\":%u,\"color\":%u,\"size\":%u,\"text\":",a[0],a[1],a[3],a[5]);break;
 case 0x10bd0:case 0x109e4:fprintf(f,"{\"op\":\"numeric\",\"jpeg_digits\":%s,\"text\":",address==0x109e4?"true":"false");break;
 case 0x8266:fprintf(f,"{\"op\":\"measurement_char\",\"x\":%u,\"y\":%u,\"char\":%u,\"size\":%u,\"color\":%u}\n",a[0],a[1],a[2],a[3],a[4]);return;
 case 0x83d6:fprintf(f,"{\"op\":\"ascii_char\",\"x\":%u,\"y\":%u,\"char\":%u,\"size\":%u,\"color\":%u}\n",a[0],a[1],a[2],a[3],a[4]);return;
 case 0x1a360:fprintf(f,"{\"op\":\"bitmap\",\"rom\":%u,\"x\":%u,\"y\":%u,\"color\":%u,\"background\":%u}\n",a[0],a[1],a[2],a[3],a[4]);return;
 case 0x8a02:fprintf(f,"{\"op\":\"fill\",\"x\":%u,\"y\":%u,\"x1\":%u,\"y1\":%u,\"color\":%u}\n",a[0],a[1],a[2],a[3],a[4]);return;
 case 0x8194:fprintf(f,"{\"op\":\"border\",\"x\":%u,\"y\":%u,\"x1\":%u,\"y1\":%u,\"color\":%u}\n",a[0],a[1],a[2],a[3],a[4]);return;
 default:return;
 }
 json_string(f,text);fputs("}\n",f);
}
bool simulator_trace(dmt_simulator *s,const char *path)
{
 if(s->draw_trace){fclose(s->draw_trace);s->draw_trace=0;}
 if(!path)return true;
 s->draw_trace=fopen(path,"w");return s->draw_trace!=0;
}
void simulator_trace_marker(dmt_simulator *s,const char *kind,const char *name)
{
 if(!s->draw_trace)return;
 fprintf(s->draw_trace,"{\"op\":");json_string(s->draw_trace,kind);fputs(",\"name\":",s->draw_trace);json_string(s->draw_trace,name);
 fprintf(s->draw_trace,",\"view\":%u,\"layout\":%u,\"mode\":%u,\"run\":%u}\n",s->app.control.view,s->app.control.layout,s->app.control.mode,s->app.run);
 fflush(s->draw_trace);
}
static bool logo_failed(void *p)
{dmt_simulator *s=p;FRESULT last;return resource_optional_logo(&s->resources,&s->filesystem,&last);}
static void delay(void *p,uint32_t ms){((dmt_simulator *)p)->elapsed_ms+=ms;}
static bool wire_tx(void *p,const uint8_t packet[10])
{dmt_simulator *s=p;memcpy(s->tx,packet,10);if(s->tx_logged<64)memcpy(s->tx_log[s->tx_logged++],packet,10);s->tx_count++;return true;}
static void tx(void *p,const uint8_t packet[10])
{dmt_simulator *s=p;if(!asic_transmit_observed(&s->asic,packet,10))s->rejected_tx++;}
static bool pin_high(void *p,uint16_t mask){return !(((dmt_simulator *)p)->pressed_pins&mask);}
static void button_delay(void *p,uint16_t ms){delay(p,ms);}
static bool longpress(void *p,uint8_t event){(void)event;return ((dmt_simulator *)p)->longpress;}
static bool charging(void *p){return ((dmt_simulator *)p)->charging;}
static uint8_t battery(void *p){return ((dmt_simulator *)p)->battery;}
static void buzzer(void *p,bool on){((dmt_simulator *)p)->buzzer=on;}
static void monitor(void *p,uint8_t mode,float value,bool f,uint8_t layout)
{dmt_simulator *s=p;monitor_alarm_render(&s->screen.view,s->app.settings.halfwords+9,mode,value,f,layout,&s->app.alarm);}
static void backlight(void *p,uint8_t compare){((dmt_simulator *)p)->backlight=compare;}
static void latch(void *p,bool on){((dmt_simulator *)p)->power=on;}
static bool settings_read(void *p,uint32_t address,uint16_t *out,uint16_t count)
{
 dmt_simulator *s=p;if(address!=DMT_SETTINGS_ADDRESS || count!=128)return false;
 memcpy(out,s->stored_settings.halfwords,256);return true;
}
static bool settings_begin(void *p){(void)p;return true;}
static bool settings_erase(void *p,uint32_t address)
{if(address!=DMT_SETTINGS_ADDRESS)return false;memset(&((dmt_simulator *)p)->stored_settings,255,256);return true;}
static uint32_t settings_write(void *p,uint32_t address,uint16_t value)
{
 dmt_simulator *s=p;if(address<DMT_SETTINGS_ADDRESS || address>=DMT_SETTINGS_ADDRESS+256 || (address&1))return 0;
 s->stored_settings.halfwords[(address-DMT_SETTINGS_ADDRESS)/2]=value;s->settings_writes++;return 4;
}
static void settings_end(void *p){(void)p;}
static bool last_read(void *p,uint8_t out[16])
{dmt_simulator *s=p;s->last_reads++;memcpy(out,s->last_record,16);return true;}
static bool last_write(void *p,const uint8_t record[16])
{dmt_simulator *s=p;memcpy(s->last_record,record,16);s->last_writes++;return true;}
static bool unresolved(void *p,uint32_t address,const dmt_application *app)
{
 (void)p;(void)address;(void)app;
 /* Returning false leaves an explicit original-address counter in the app. */
 return false;
}
#if DMT_THEMES
static unsigned theme_list_cb(void *c,uint8_t *slots,unsigned max){(void)c;return theme_list(slots,max);}
static bool theme_info_cb(void *c,uint8_t slot,dmt_theme_info *i){return theme_info(slot,i,&((dmt_simulator *)c)->resources.file);}
static uint8_t theme_active_cb(void *c){return ((dmt_simulator *)c)->theme.slot;}
static bool theme_select_cb(void *c,uint8_t slot)
{dmt_simulator *s=c;if(!theme_activate(&s->theme,slot,&s->resources.file))return false;theme_settings_store(s->app.settings.halfwords,slot);return true;}
static enum dmt_theme_result theme_remove_cb(void *c,uint8_t slot,const dmt_theme_progress *p){(void)c;return theme_remove(slot,p);}
static uint32_t theme_free_cb(void *c){(void)c;return theme_free_bytes();}
static void simple_get_cb(void *c,dmt_simple_config *cfg){simple_config_load(((dmt_simulator *)c)->app.settings.halfwords,cfg);}
static void simple_set_cb(void *c,const dmt_simple_config *cfg){simple_config_store(((dmt_simulator *)c)->app.settings.halfwords,cfg);}
#endif
void simulator_writable(dmt_simulator *s)
{dmt_filesystem_io fio={s,flash_read,flash_write,flash_gap};filesystem_bind(&fio);s->writable=true;}
bool simulator_flash_save(const dmt_simulator *s,const char *path,const char *source)
{
 if(!s->flash || (source && !strcmp(path,source)))return false;
 FILE *f=fopen(path,"wbx");if(!f)return false;
 bool ok=fwrite(s->flash,1,8388608,f)==8388608;if(fclose(f))ok=false;if(!ok)remove(path);return ok;
}
bool simulator_open(dmt_simulator *s,const char *path)
{
 memset(s,0,sizeof *s);FILE *f=fopen(path,"rb");if(!f)return false;
 s->flash=malloc(8388608);bool ok=s->flash && fread(s->flash,1,8388608,f)==8388608 && fgetc(f)==EOF;
 fclose(f);if(!ok){free(s->flash);s->flash=0;return false;}
#if DMT_THEMES
 s->battery=87;s->canvas=(dmt_canvas){s,themed_pixel,320,240,0};
 s->jpeg_canvas=(dmt_canvas){s,pixel,320,240,0};s->resources.canvas=&s->jpeg_canvas;
#else
 s->battery=87;s->canvas=(dmt_canvas){s,pixel,320,240,0};s->resources.canvas=&s->canvas;
#endif
 s->status=(dmt_status_view){s,image,delay,&s->canvas};
 s->settings_backend=(dmt_settings_backend){s,settings_read,settings_begin,settings_erase,settings_write,settings_end};
 s->last_backend=(dmt_last_mode_backend){s,last_read,last_write,0};
 s->asic=(dmt_asic_io){s,wire_tx,delay};s->diagnostic_buttons=(dmt_button_io){.context=s,.pin_high=pin_high,.delay_ms=button_delay};
 s->io=(dmt_application_io){.context=s,.send=tx,.longpress=longpress,.delay_ms=delay,.charging=charging,.battery_read=battery,.buzzer=buzzer,.backlight=backlight,.power_latch=latch,.unresolved=unresolved,.internal_flash=&s->settings_backend,.canvas=&s->canvas,.status_view=&s->status,.reading=&s->screen.reading,.view=&s->screen.view,.last_mode=&s->last_backend,.screen=&s->screen,.diagnostic_buttons=&s->diagnostic_buttons};
#if DMT_THEMES
 s->theme_io=(dmt_theme_io){s,theme_list_cb,theme_info_cb,theme_active_cb,theme_select_cb,theme_remove_cb,theme_free_cb,simple_get_cb,simple_set_cb};
 s->io.themes=&s->theme_io;
#endif
 dmt_filesystem_io fio={s,flash_read,0,flash_gap};filesystem_bind(&fio);
 if(filesystem_mount(&s->filesystem,0)!=FR_OK){/* Original logo volume may be damaged; resource volume is required. */}
 if(filesystem_mount(&s->filesystem,1)!=FR_OK){simulator_close(s);return false;}
 settings_image_defaults(&s->stored_settings);s->stored_settings.halfwords[0]=0;
 settings_access(settings_image_word(&s->stored_settings),SETTINGS_THEME,1,1);
 settings_access(settings_image_word(&s->stored_settings),SETTINGS_LAYOUT,1,0);
 settings_language_set(settings_image_word(&s->stored_settings),1);
 return true;
}
void simulator_close(dmt_simulator *s)
{simulator_trace(s,0);f_mount(0,"0:",0);f_mount(0,"1:",0);filesystem_bind(0);free(s->flash);s->flash=0;}
void simulator_power_on(dmt_simulator *s)
{
 memset(s->pixels,0,sizeof s->pixels);memset(&s->screen,0,sizeof s->screen);
 s->screen.view=(dmt_view){&s->status,s,view_trace_draw,buzzer,monitor};s->screen.context=s;s->screen.logo_failed=logo_failed;s->screen.delay_ms=delay;s->screen.backlight=backlight;
 screen_power_on(&s->screen);s->power=true;
#if DMT_THEMES
 {
  static union {uint32_t align;uint8_t bytes[4096];} scratch;
  dmt_theme_boot tb={&s->filesystem,&s->resources.file,scratch.bytes,sizeof scratch.bytes,&s->canvas,delay,s};
  bool installed;enum dmt_theme_result result=DMT_THEME_OK;
  s->theme_slot=theme_boot(&tb,theme_settings_slot(s->stored_settings.halfwords),&s->theme,&installed,&result);
  if(installed)fprintf(stderr,"THEME.PKG: %s (slot %u)\n",theme_result_text(result),s->theme_slot);
  if(s->preview_palette_count){
   s->theme.palette_count=s->preview_palette_count;
   memcpy(s->theme.palette,s->preview_palette,sizeof s->theme.palette);
  }
 }
#endif
 application_init(&s->app,0,&s->io);
#if DMT_THEMES
 theme_settings_store(s->app.settings.halfwords,s->theme_slot);
#endif
 application_step(&s->app,&s->io);
}
bool simulator_configure(dmt_simulator *s,unsigned view,unsigned layout,unsigned english)
{
 if(view<1 || view>3 || layout>1 || english>1)return false;
 settings_access(settings_image_word(&s->stored_settings),SETTINGS_THEME,1,view);
 settings_access(settings_image_word(&s->stored_settings),SETTINGS_LAYOUT,1,layout);
 settings_language_set(settings_image_word(&s->stored_settings),english);simulator_power_on(s);return true;
}
bool simulator_button(dmt_simulator *s,unsigned event,bool is_long)
{
 if(!s->power || event<1 || event>7)return false;
 static const uint16_t pins[]={0x400,0x800,0x200,0x1000,0x100,0x100,4};
 s->pressed_pins=pins[event-1];s->longpress=is_long;s->app.control.event=(uint8_t)event;
 application_step(&s->app,&s->io);s->longpress=false;s->pressed_pins=0;return true;
}
bool simulator_diagnostic_boot(dmt_simulator *s)
{
 memset(s->pixels,0,sizeof s->pixels); /* display_init's power-on clear */
 s->pressed_pins=0xc00;s->power=true;
 bool ok=application_boot(&s->app,0,&s->io);s->pressed_pins=0;return ok;
}
void simulator_packet(dmt_simulator *s,const uint8_t packet[13])
{if(s->power){s->packets++;application_receive_idle(&s->app,packet);application_step(&s->app,&s->io);}}
void simulator_ack(dmt_simulator *s,uint8_t mode)
{uint8_t packet[13]={0x5a,0xa5,0xfd,mode};simulator_packet(s,packet);}
bool simulator_reading(dmt_simulator *s,const char *text,unsigned unit)
{
 uint8_t packet[13]={0x5a,0xa5},segments[4];unsigned n=0,decimal=0;bool negative=false;
 if(*text=='-' || *text=='+'){negative=*text=='-';text++;}
 for(;*text;text++){
  if(*text=='.'){if(decimal || !n || n>3)return false;decimal=n;continue;}
  if(n>=4 || *text<'0' || *text>'9')return false;
  bool found=false;for(unsigned c=0;c<256;c++)if(!(c&16) && measurement_segment_decode((uint8_t)c)==(uint8_t)(*text-'0')){segments[n++]=c;found=true;break;}
  if(!found)return false;
 }
 if(n!=4)return false;
 for(unsigned i=0;i<4;i++){packet[2+i]|=segments[i]&0xf0;packet[3+i]|=segments[i]&15;}
 if(negative)packet[2]|=16;
 if(decimal)packet[2+decimal]|=16;
 switch(unit){
 case 6:case 7:packet[8]|=2;if(unit==7)packet[7]|=1;break;
 case 8:case 9:case 10:packet[8]|=1;if(unit==8)packet[6]|=16;if(unit==9)packet[7]|=1;break;
 case 11:case 12:case 13:packet[7]|=32;if(unit==12)packet[6]|=64;if(unit==13)packet[7]|=4;break;
 case 15:case 16:case 17:packet[7]|=16;if(unit==15)packet[6]|=32;if(unit==16)packet[6]|=16;break;
 case 18:case 19:case 20:packet[8]|=4;if(unit==19)packet[6]|=64;if(unit==20)packet[7]|=4;break;
 case 21:packet[8]|=32;break;
 default:return false;
 }
 simulator_packet(s,packet);return true;
}
bool simulator_snapshot(dmt_simulator *s,const char *path)
{simulator_trace_marker(s,"snapshot",path);return simulator_png(path,s->pixels,320,240);}
enum {STATE_BYTES=8+256+16};
static const uint8_t state_magic[8]={'D','M','T','S','I','M',4,0};
bool simulator_state_load(dmt_simulator *s,const char *path)
{
 FILE *f=fopen(path,"rb");if(!f)return errno==ENOENT;
 uint8_t b[STATE_BYTES];bool ok=fread(b,1,sizeof b,f)==sizeof b && fgetc(f)==EOF && !memcmp(b,state_magic,8);fclose(f);if(!ok)return false;
 /* Explicit little-endian host-state format, independent of host architecture. */
 for(unsigned i=0;i<128;i++)s->stored_settings.halfwords[i]=(uint16_t)(b[8+i*2]|b[9+i*2]<<8);
 /* Host container preserves opaque bytes across profiles. STOCK never invokes
  * the feature backend or interprets this record. */
 memcpy(s->last_record,b+264,16);
 return true;
}
bool simulator_state_save(const dmt_simulator *s,const char *path)
{
 /* Validate an existing destination before replacing it. */
 FILE *old=fopen(path,"rb");if(old){uint8_t magic[8];bool ok=fread(magic,1,8,old)==8 && !memcmp(magic,state_magic,8);fclose(old);if(!ok)return false;}else if(errno!=ENOENT)return false;
 size_t len=strlen(path);char *tmp=malloc(len+5);if(!tmp)return false;memcpy(tmp,path,len);memcpy(tmp+len,".tmp",5);
 FILE *f=fopen(tmp,"wbx");if(!f){free(tmp);return false;}
 uint8_t b[STATE_BYTES]={0};memcpy(b,state_magic,8);for(unsigned i=0;i<128;i++){b[8+i*2]=(uint8_t)s->stored_settings.halfwords[i];b[9+i*2]=(uint8_t)(s->stored_settings.halfwords[i]>>8);}
 memcpy(b+264,s->last_record,16);
 bool ok=fwrite(b,1,sizeof b,f)==sizeof b;if(fclose(f))ok=false;if(ok)ok=rename(tmp,path)==0;if(!ok)remove(tmp);free(tmp);return ok;
}
