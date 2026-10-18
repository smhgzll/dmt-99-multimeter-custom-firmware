#define _POSIX_C_SOURCE 200809L
#include "simulator.h"
#include "config.h"
#include "device_link.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static dmt_simulator sim;
static const char *flash_path;
static void status(void)
{
 printf("{\"profile\":\"%s\",\"run\":%u,\"power\":%s,\"view\":%u,\"layout\":%u,\"mode\":%u,\"unit\":%u,\"value\":%.9g,\"hold\":%u,\"relative\":%u,\"menu\":%u,\"history\":%u,\"tx_count\":%u,\"tx_mode\":%u,\"last_reads\":%u,\"last_writes\":%u,\"images\":%u,\"resource_errors\":%u,\"unresolved_calls\":%u,\"last_unresolved\":%u,\"elapsed_ms\":%llu}\n",
 DMT_BUILD_NAME,sim.app.run,sim.power?"true":"false",sim.app.control.view,sim.app.control.layout,sim.app.control.mode,sim.app.control.unit,(double)sim.app.control.value,sim.app.control.hold,sim.app.control.relative,sim.app.menu.state,sim.app.history.count,sim.tx_count,sim.tx[3],sim.last_reads,sim.last_writes,sim.resources.images_opened,sim.resource_errors,sim.app.unresolved_calls,sim.app.last_unresolved_address,(unsigned long long)sim.elapsed_ms);
}
static void help(void)
{
 puts("Commands: status; button EVENT [long]; ack MODE; reading [-]DD.DD UNIT;\n"
      "packet HEX26; tick COUNT; step; charge 0|1; battery 0..100; txlog;\n"
      "snapshot PATH.png; trace PATH.jsonl|off; marker NAME; flash-save NEW.bin (with --writable); poweroff; poweron; powercycle; configure VIEW LAYOUT ENGLISH; diagnostic; quit.\n"
      "Integers accept 0x notation. configure is a simulator fixture, not a meter button.\n"
      "ACKs are explicit; the simulator does not invent measurement ASIC responses.");
}
static bool number(const char *text,unsigned *n)
{if(!text || !*text)return false;char *end;unsigned long v=strtoul(text,&end,0);if(*end || v>1000000)return false;*n=(unsigned)v;return true;}
static int command(char *line)
{
 line[strcspn(line,"\r\n")]=0;char *verb=strtok(line," \t");if(!verb || *verb=='#')return 0;
 char *a=strtok(0," \t"),*b=strtok(0," \t"),*c=strtok(0," \t");unsigned n=0,m=0,k=0;
 if(!strcmp(verb,"quit"))return 1;
 if(!strcmp(verb,"help")){help();return 0;}
 if(!strcmp(verb,"status")){status();return 0;}
 if(!strcmp(verb,"txlog")){printf("{\"tx\":[");for(unsigned i=0;i<sim.tx_logged;i++){printf("%s\"",i?",":"");for(unsigned j=0;j<10;j++)printf("%02x",sim.tx_log[i][j]);printf("\"");}printf("]}\n");return 0;}
 if(!strcmp(verb,"button") && number(a,&n) && (!b || !strcmp(b,"long")))return simulator_button(&sim,n,b!=0)?0:-1;
 if(!strcmp(verb,"ack") && number(a,&n) && n<=255){simulator_ack(&sim,(uint8_t)n);return 0;}
 if(!strcmp(verb,"reading") && a && number(b,&n))return simulator_reading(&sim,a,n)?0:-1;
 if(!strcmp(verb,"packet") && a && strlen(a)==26){
  uint8_t packet[13];for(unsigned i=0;i<13;i++){char byte[3]={a[i*2],a[i*2+1],0},*end;unsigned long v=strtoul(byte,&end,16);if(end!=byte+2)return -1;packet[i]=(uint8_t)v;}simulator_packet(&sim,packet);return 0;
 }
 if(!strcmp(verb,"tick") && number(a,&n)){for(unsigned i=0;i<n;i++)if(sim.power)application_timer(&sim.app,&sim.io);return 0;}
 if(!strcmp(verb,"step")){application_step(&sim.app,&sim.io);return 0;}
 if(!strcmp(verb,"diagnostic") && !a)return simulator_diagnostic_boot(&sim)?0:-1;
 if(!strcmp(verb,"charge") && number(a,&n) && n<=1){sim.charging=n;application_step(&sim.app,&sim.io);return 0;}
 if(!strcmp(verb,"battery") && number(a,&n) && n<=100){sim.battery=(uint8_t)n;return 0;}
 if(!strcmp(verb,"snapshot") && a)return simulator_snapshot(&sim,a)?0:-1;
 if(!strcmp(verb,"trace") && a && !b)return simulator_trace(&sim,strcmp(a,"off")?a:0)?0:-1;
 if(!strcmp(verb,"marker") && a && !b){simulator_trace_marker(&sim,"marker",a);return 0;}
 if(!strcmp(verb,"flash-save") && a && !b)return simulator_flash_save(&sim,a,flash_path)?0:-1;
 if(!strcmp(verb,"poweroff")){if(sim.power)application_power_down(&sim.app,&sim.io);return 0;}
 if(!strcmp(verb,"poweron")){if(!sim.power)simulator_power_on(&sim);return 0;}
 if(!strcmp(verb,"powercycle")){if(sim.power)application_power_down(&sim.app,&sim.io);simulator_power_on(&sim);return 0;}
 if(!strcmp(verb,"configure") && number(a,&n) && number(b,&m) && number(c,&k))return simulator_configure(&sim,n,m,k)?0:-1;
 return -1;
}
int main(int argc,char **argv)
{
 const char *flash="../flashdump/dmt99_flash_1.bin",*script=0,*state=0,*trace=0,*mcu_image=0;
 bool device=false,writable=false;
 const char *palette=0;
 bool quiet=false;
 for(int i=1;i<argc;i++){
  if(!strcmp(argv[i],"--pipe")){quiet=true;continue;}
  if(!strcmp(argv[i],"--device-pty")){device=true;quiet=true;continue;}
  if(!strcmp(argv[i],"--writable")){writable=true;continue;}
  if(!strcmp(argv[i],"--help")){printf("%s [--flash PATH] [--script PATH] [--state SIMULATOR_STATE] [--trace DRAWS.jsonl] [--device-pty [--mcu-image APP.bin]] [--writable] [--palette RRRR:RRRR,...]\n",argv[0]);help();return 0;}
  if(i+1>=argc){fprintf(stderr,"Missing option argument\n");return 2;}
  if(!strcmp(argv[i],"--flash"))flash=argv[++i];else if(!strcmp(argv[i],"--script"))script=argv[++i];else if(!strcmp(argv[i],"--state"))state=argv[++i];else if(!strcmp(argv[i],"--trace"))trace=argv[++i];else if(!strcmp(argv[i],"--mcu-image"))mcu_image=argv[++i];else if(!strcmp(argv[i],"--palette"))palette=argv[++i];else{fprintf(stderr,"Unknown option\n");return 2;}
 }
 flash_path=flash;
 if(!simulator_open(&sim,flash)){fprintf(stderr,"Cannot load 8 MiB Flash resources\n");return 2;}
 if(writable)simulator_writable(&sim);
#if DMT_THEMES
 /* --palette fd40:07e0,ffff:ffe0 previews theme colours (RGB565 hex pairs). */
 for(const char *p=palette;p && *p && sim.preview_palette_count<DMT_THEME_PALETTE;){
  char *end;unsigned long from=strtoul(p,&end,16);
  if(*end!=':'){fprintf(stderr,"Invalid --palette\n");simulator_close(&sim);return 2;}
  unsigned long to=strtoul(end+1,&end,16);
  if((*end && *end!=',') || from>0xffff || to>0xffff){fprintf(stderr,"Invalid --palette\n");simulator_close(&sim);return 2;}
  sim.preview_palette[sim.preview_palette_count][0]=(uint16_t)from;sim.preview_palette[sim.preview_palette_count++][1]=(uint16_t)to;
  p=*end?end+1:end;
 }
#else
 if(palette){fprintf(stderr,"--palette needs the release simulator\n");simulator_close(&sim);return 2;}
#endif
 if(state && !simulator_state_load(&sim,state)){fprintf(stderr,"Invalid simulator state file; unchanged\n");simulator_close(&sim);return 2;}
 if(trace && !simulator_trace(&sim,trace)){perror(trace);simulator_close(&sim);return 2;}
 FILE *input=script?fopen(script,"r"):stdin;if(!input){perror(script);simulator_close(&sim);return 2;}
 simulator_power_on(&sim);if(!script && !quiet)help();char line[1024];unsigned row=0;int result=0;
 if(device){
  /* Simulated device: protocol on a pseudo terminal, commands (readings, keys) on stdin. */
  static device_link link;
  if(!device_link_open(&link,&sim,mcu_image)){fprintf(stderr,"Cannot open the simulated device link\n");simulator_close(&sim);return 2;}
  printf("{\"device_pty\":\"%s\"}\n",link.path);fflush(stdout);
  bool open_input=true;
  setvbuf(input,0,_IONBF,0); /* poll() must see every pending line */
  for(;;){
   bool ready;device_link_service(&link,20,open_input?fileno(input):-1,&ready);
   if(!ready)continue;
   if(!fgets(line,sizeof line,input)){open_input=false;if(script)break;continue;}
   uint32_t before=sim.packets;int rc=command(line);if(sim.packets!=before)device_link_packet(&link);
   fflush(stdout);if(rc==1)break;if(rc<0)fprintf(stderr,"Invalid command\n");
  }
  device_link_close(&link);simulator_close(&sim);return 0;
 }
 while(fgets(line,sizeof line,input)){
  row++;int rc=command(line);fflush(stdout);if(rc<0){fprintf(stderr,"Invalid command or operation failed at line %u\n",row);result=2;break;}if(rc==1)break;
 }
 if(script)fclose(input);
 if(!result && state && !simulator_state_save(&sim,state)){fprintf(stderr,"Cannot save simulator state\n");result=2;}
 simulator_close(&sim);return result;
}
