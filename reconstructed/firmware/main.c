/* Target entry of the standalone DMT99 firmware.
 *
 * Start-up follows main 0x908a call for call:
 *   NVIC_PriorityGroupConfig(2), SystemCoreClockUpdate, delay_init,
 *   debug USART2 115200, measurement USART1 9600 + DMA, keys/power latch,
 *   battery ADC + charger input, panel, resources (SPI Flash, FatFs), TIM3
 *   (ARR 9999, PSC 14399: 1 s at 144 MHz), application loop 0x8ed4.
 * The loop uses the cooperative application in application/application.c,
 * whose states stand for the original's blocking menu, editor, language,
 * history and charging loops. Each iteration first moves interrupt-captured
 * input (UART snapshot, TIM3 ticks, power-key request) into the application,
 * then runs one application_step, which performs the 8ed4 body: key handlers
 * per view, frame parse/event dispatch, charger poll and battery refresh.
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "runtime.h"
#include "display_port.h"
#include "storage_port.h"
#include "wch_platform.h"
#include "wch_runtime.h"
#include "resource_volume.h"
#include "monitor.h"
#if DMT_THEMES
#include "theme_ui.h"
#endif

dmt_application fw_app;
uint8_t fw_dma_frame[13];
volatile fw_rx_shadow fw_rx;
volatile uint8_t fw_timer_pending,fw_shutdown_request,fw_shutdown_age;
#if DMT_THEMES
volatile uint8_t fw_theme_busy;
#endif
fw_counters_t fw_counters;

static dmt_screen screen;
static dmt_resources resources;
static FATFS filesystem;
static dmt_canvas canvas;
static dmt_status_view status;
static dmt_application_io io;

/* ---- keys (7944 pins, 7b08/7c0a/7c42/7cd8 behaviour in ui/buttons.c) ---- */
static bool pin_high(void *c,uint16_t mask){(void)c;return (GPIOC->INDR&mask)!=0;}
static void key_delay(void *c,uint16_t ms){(void)c;fw_delay_ms(ms);}
static void clear_exti(void *c,uint16_t mask){(void)c;EXTI->INTFR=mask;}
static void request_power_down(void *c)
{(void)c;if(!fw_shutdown_request){fw_shutdown_age=0;fw_shutdown_request=1;}}
const dmt_button_io fw_button_io={0,pin_high,key_delay,clear_exti,request_power_down};
bool fw_normal_operation(void){return screen.normal;}

/* ---- application callbacks ---- */
static void delay_cb(void *c,uint32_t ms){(void)c;fw_delay_ms(ms);}
#if DMT_FW_BRINGUP
static void hex_line(const char *tag,const uint8_t *p,unsigned n)
{
    char line[64];unsigned k=0;
    for(unsigned i=0;i<n && k+3<sizeof line;++i){static const char h[]="0123456789abcdef";line[k++]=h[p[i]>>4];line[k++]=h[p[i]&15];line[k++]=' ';}
    line[k]=0;fw_printf("[%lu] %s %s\r\n",(unsigned long)fw_millis(),tag,line);
}
#endif
static void send(void *c,const uint8_t p[10])
{
    (void)c;bool ok=recovered_asic_transmit(0,p);
    if(ok)fw_counters.tx_sent++;else fw_counters.tx_rejected++;
#if DMT_FW_BRINGUP
    hex_line(ok?"tx":"tx-rejected",p,10);
#endif
}
static bool longpress(void *c,uint8_t key){(void)c;return buttons_wait_longpress(&fw_button_io,key,&fw_app.control.event);}
static bool charging(void *c){(void)c;return fw_charging();}
static int battery_level(void *c){(void)c;return fw_battery_percent();}
static uint8_t battery_byte(void *c){(void)c;return (uint8_t)fw_battery_percent();}
static void buzzer(void *c,bool on){(void)c;recovered_buzzer_enable(on);}
static void backlight(void *c,uint8_t compare){(void)c;recovered_backlight_compare(compare);}
static void power_latch(void *c,bool on)
{(void)c;if(on)GPIOC->BSHR=GPIO_Pin_1;else GPIOC->BCR=GPIO_Pin_1;}
#if DMT_THEMES
/* Active theme overlay (ui/theme.c) and the theme manager's operations. */
static dmt_theme_active theme;
static bool theme_image(const char *path,uint16_t x,uint16_t y)
{char themed[96];return theme_path(&theme,path,themed,sizeof themed) && resource_draw_jpeg(&resources,themed,x,y)==0;}
static unsigned theme_list_cb(void *c,uint8_t *slots,unsigned max){(void)c;return theme_list(slots,max);}
static bool theme_info_cb(void *c,uint8_t slot,dmt_theme_info *i){(void)c;return theme_info(slot,i,&resources.file);}
static uint8_t theme_active_cb(void *c){(void)c;return theme.slot;}
static bool theme_select_cb(void *c,uint8_t slot)
{(void)c;if(!theme_activate(&theme,slot,&resources.file))return false;theme_settings_store(fw_app.settings.halfwords,slot);return true;}
static enum dmt_theme_result theme_remove_cb(void *c,uint8_t slot,const dmt_theme_progress *p)
{(void)c;fw_theme_busy=1;enum dmt_theme_result r=theme_remove(slot,p);fw_theme_busy=0;return r;}
static uint32_t theme_free_cb(void *c){(void)c;return theme_free_bytes();}
static void simple_get_cb(void *c,dmt_simple_config *cfg){(void)c;simple_config_load(fw_app.settings.halfwords,cfg);}
static void simple_set_cb(void *c,const dmt_simple_config *cfg){(void)c;simple_config_store(fw_app.settings.halfwords,cfg);}
static const dmt_theme_io theme_io={0,theme_list_cb,theme_info_cb,theme_active_cb,theme_select_cb,theme_remove_cb,theme_free_cb,
    simple_get_cb,simple_set_cb};
#endif
static bool image(void *c,const char *path,uint16_t x,uint16_t y)
{
    (void)c;
#if DMT_THEMES
    if(theme_image(path,x,y))return false;
#endif
    return resource_draw_jpeg(&resources,path,x,y)!=0;
}
static bool logo_failed(void *c){(void)c;FRESULT last;return resource_optional_logo(&resources,&filesystem,&last);}
static void monitor(void *c,uint8_t mode,float value,bool fahrenheit,uint8_t layout)
{(void)c;monitor_alarm_render(&screen.view,fw_app.settings.halfwords+9,mode,value,fahrenheit,layout,&fw_app.alarm);}
static void debug_text(void *c,const char *text)
{(void)c;size_t n=0;while(text[n])n++;fw_debug_write(text,n);}
static bool unresolved(void *c,uint32_t address,const dmt_application *a)
{
    (void)c;(void)a;
    if(address==0xde12){FRESULT r;resource_remove_logo(&filesystem,&r);return true;}  /* dc46 -> de12 */
    fw_counters.unresolved++;fw_counters.last_unresolved=address;
    fw_printf("unresolved original routine 0x%05lx\r\n",(unsigned long)address);
    return false;
}

static void fill(void *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{
    /* 0x876a for the whole screen, 0x8a02 otherwise. */
    if(!x && !y && x1==319 && y1==239)display_solid(c,color);
    else display_rectangle_fill(c,x,y,x1,y1,color);
}
#if DMT_THEMES
/* Firmware-drawn primitives go through the active theme's palette; JPEG
 * output uses the display path directly and is not recoloured. */
static void theme_pixel(void *c,uint16_t x,uint16_t y,uint16_t color){display_pixel(c,x,y,theme_color(&theme,color));}
static void theme_fill(void *c,uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color)
{fill(c,x,y,x1,y1,theme_color(&theme,color));}
#endif
static void bind(void)
{
#if DMT_THEMES
    canvas=(dmt_canvas){&fw_display,theme_pixel,320,240,theme_fill};
#else
    canvas=(dmt_canvas){&fw_display,display_pixel,320,240,fill};
#endif
    resources.canvas=&canvas;resources.display=&fw_display;
    status=(dmt_status_view){0,image,delay_cb,&canvas};
    screen.view=(dmt_view){&status,0,0,buzzer,monitor};
    screen.logo_failed=logo_failed;screen.delay_ms=delay_cb;screen.backlight=backlight;screen.debug=debug_text;
    screen_power_on(&screen);
    io=(dmt_application_io){.send=send,.longpress=longpress,.delay_ms=delay_cb,.charging=charging,
        .battery_read=battery_byte,.buzzer=buzzer,.backlight=backlight,.power_latch=power_latch,
        .unresolved=unresolved,.internal_flash=&fw_settings_backend,.canvas=&canvas,.status_view=&status,
        .reading=&screen.reading,.view=&screen.view,.screen=&screen,.diagnostic_buttons=&fw_button_io,
        .battery_level=battery_level};
#if DMT_LAST_MODE
    io.last_mode=&fw_last_mode_backend;
#endif
#if DMT_THEMES
    io.themes=&theme_io;
#endif
}

/* Move interrupt-captured frames into the application (c468 semantics). */
static void receive(void)
{
    uint8_t frame[13],event[13];bool have_frame,have_event;uint8_t suppressed;
    __asm__ volatile("csrc mstatus, 8");
    have_frame=fw_rx.frame_ready;have_event=fw_rx.event_pending;suppressed=fw_rx.rx_suppressed;
    for(unsigned i=0;i<13;++i){frame[i]=fw_rx.frame[i];event[i]=fw_rx.saved_event[i];}
    fw_rx.frame_ready=0;fw_rx.event_pending=0;
    __asm__ volatile("csrs mstatus, 8");
    if(have_frame){for(unsigned i=0;i<13;++i)fw_app.frame[i]=frame[i];fw_app.rx_suppressed=suppressed;}
#if DMT_FW_BRINGUP
    if(have_frame)hex_line(suppressed?"rx-blank":"rx",frame,13);
#endif
    if(have_event){for(unsigned i=0;i<13;++i)fw_app.saved_event[i]=event[i];fw_app.event_consumed=0;}
}
static void __attribute__((noreturn)) halt(void)
{
    /* c320 ends in an endless loop after releasing PC1; power normally drops. */
    __asm__ volatile("csrc mstatus, 8");
    for(;;)__asm__ volatile("wfi");
}
void fw_emergency_power_off(void)
{
    fw_counters.emergency_off++;
    recovered_backlight_compare(100);
    GPIOC->BCR=GPIO_Pin_1;
    halt();
}

int main(void)
{
    recovered_nvic_priority_group(2);   /* NVIC_PriorityGroupConfig(2) */
    SystemCoreClockUpdate();
    fw_delay_init();
    fw_debug_init(115200);
    fw_fault_boot_report();
    recovered_measurement_uart_init(9600,fw_dma_frame);
    recovered_buttons_init(delay_cb,0);
    fw_battery_inputs_init();
    fw_display_init();
    bind();
    fw_resources_init(&filesystem,resources.pool.bytes);
    fw_printf("flash jedec=%06lx size=%lu fdisk=%d mount0=%d label=%d mount1=%d skipped=%lu written=%lu\r\n",
        (unsigned long)fw_storage.jedec,(unsigned long)fw_storage.capacity,fw_storage.fdisk,fw_storage.mount0,
        fw_storage.label,fw_storage.mount1,(unsigned long)fw_storage.writes_skipped,(unsigned long)fw_storage.sectors_written);
    recovered_periodic_timer_init(9999,14399);

#if DMT_FW_DIAGNOSTIC_ENTRY
    application_boot(&fw_app,0,&io);
#else
    /* PC10+PC11 at power-on selects 8ed4's diagnostic loop, which sends the
     * uninterpreted A0 command to the measurement IC. Disabled by default. */
#if DMT_THEMES
    /* Install a pending THEME.PKG from the bootloader disk, then activate the
     * selected theme (settings halfwords 118/119) before anything is drawn. */
    dmt_theme_boot theme_start={&filesystem,&resources.file,resources.pool.bytes,sizeof resources.pool.bytes,&canvas,delay_cb,0};
    bool theme_installed;enum dmt_theme_result theme_result=DMT_THEME_OK;
    fw_theme_busy=1;
    uint8_t theme_slot=theme_boot(&theme_start,theme_settings_slot((const uint16_t *)0x08040000u),&theme,&theme_installed,&theme_result);
    fw_theme_busy=0;
#endif
    application_init(&fw_app,0,&io);
#if DMT_THEMES
    theme_settings_store(fw_app.settings.halfwords,theme_slot);
    if(theme_installed)fw_printf("THEME.PKG: %s, active slot %u\r\n",theme_result_text(theme_result),theme_slot);
#endif
#endif
    fw_printf("settings %s, view %u, run %u, stack free %lu\r\n",fw_app.settings_loaded?"loaded":"absent",
        fw_app.control.view,fw_app.run,(unsigned long)fw_stack_unused());

#if DMT_FW_BRINGUP
    uint32_t next_status=0;
#endif
    for(;;){
        fw_counters.loops++;
#if DMT_FW_BRINGUP
        if(fw_millis()>=next_status){
            next_status=fw_millis()+2000;
            ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_239Cycles5);ADC_SoftwareStartConvCmd(ADC1,ENABLE);
            uint32_t polls=0;while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET && ++polls<100000){}
            uint16_t raw=ADC_GetConversionValue(ADC1);
            fw_printf("[%lu] status run=%u view=%u mode=0x%02x unit=%u adc10=%u (%lumV) pa8=%u keys=%03lx latch=%u frames=%lu events=%lu tx=%lu/%lu unresolved=%lu stack_free=%lu\r\n",
                (unsigned long)fw_millis(),fw_app.run,fw_app.control.view,fw_app.control.mode,fw_app.control.unit,raw,
                (unsigned long)((raw*6600u)>>12),fw_charging(),(unsigned long)((GPIOC->INDR>>2)&0x7ff),(unsigned)((GPIOC->OUTDR>>1)&1),
                (unsigned long)fw_rx.frames,(unsigned long)fw_rx.events,(unsigned long)fw_counters.tx_sent,(unsigned long)fw_counters.tx_rejected,
                (unsigned long)fw_counters.unresolved,(unsigned long)fw_stack_unused());
        }
#endif
        if(fw_shutdown_request){application_power_down(&fw_app,&io);halt();}
        receive();
        while(fw_timer_pending){
            __asm__ volatile("csrc mstatus, 8");fw_timer_pending--;__asm__ volatile("csrs mstatus, 8");
            application_timer(&fw_app,&io);
            if(fw_app.power.shutdown)halt();
        }
        application_step(&fw_app,&io);
        if(fw_app.run==DMT_STOPPED){
            if(fw_app.power.shutdown)halt();
            /* Settings could not be read (d928 boundary); nothing to run. */
            fw_printf("application stopped, unresolved 0x%05lx\r\n",(unsigned long)fw_app.last_unresolved_address);
            halt();
        }
    }
}
