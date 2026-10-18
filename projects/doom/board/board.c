/* SPDX-License-Identifier: AGPL-3.0-or-later
 * DMT99 wiring/sequence copied from the read-only POTAT-99 reconstruction.
 * Polling keys, no ASIC protocol, NOR, internal Flash or option-byte writes.
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "display.h"
#include "app.h"
#include "game.h"
#include "sound.h"
#include "controls.h"
#include "version.h"
static doom_sound audio;
static doom_app app;
static void collect_audio(void);
static bool drawing;
static char queued_keys[8];
static uint8_t queue_head,queue_count;
static doom_controls controls;
static int32_t pending_move,pending_turn;
static void poll_keys(uint32_t now);
static void input(char key);
static void dispatch(char key)
{
    if(!drawing){input(key);return;}
    if(queue_count<8){queued_keys[(queue_head+queue_count)%8]=key;++queue_count;}
    else if(key=='p')queued_keys[(queue_head+7)%8]=key;
}
static void drain_keys(void)
{
    while(queue_count){char key=queued_keys[queue_head];queue_head=(queue_head+1)%8;--queue_count;input(key);}
    /* Rendering callbacks collect only input, never mutate the world. Clear
     * first: any new samples collected inside collision are for next drain. */
    int32_t move=pending_move,turn=pending_turn;pending_move=pending_turn=0;
    if(move || turn){drawing=true;doom_app_motion(&app,move,turn);drawing=false;collect_audio();}
}

static void audio_service(void)
{if(audio.active)doom_sound_update(&audio,fw_millis());}
static void game_service(void){audio_service();poll_keys(fw_millis());}

static void set_pc(void *c,uint16_t mask,bool high)
{(void)c;if(high)GPIOC->BSHR=mask;else GPIOC->BCR=mask;}
static void write_pb(void *c,uint16_t word){(void)c;GPIOB->OUTDR=word;}
static dmt_display panel={.bus={0,set_pc,write_pb}};
static void delay(void *c,uint32_t ms){(void)c;fw_delay_ms(ms);}

void fw_debug_init(uint32_t baud)
{
    GPIO_InitTypeDef gpio={0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    gpio.GPIO_Pin=GPIO_Pin_2;gpio.GPIO_Mode=GPIO_Mode_AF_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio);
    USART2->CTLR1=0;USART2->CTLR2=0;USART2->CTLR3=0;
    USART2->BRR=(SystemCoreClock/2+baud/2)/baud;
    USART2->CTLR1=USART_CTLR1_UE|USART_CTLR1_TE;
}
void fw_debug_write(const char *p,size_t n)
{for(size_t i=0;i<n;++i){while(!(USART2->STATR&USART_FLAG_TC)){}USART2->DATAR=(uint8_t)p[i];}}
static void say(const char *p){size_t n=0;while(p[n])++n;fw_debug_write(p,n);}

static void brightness(uint16_t compare)
{
    TIM_OCInitTypeDef pwm={0};TIM_TimeBaseInitTypeDef timer={0};GPIO_InitTypeDef gpio={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_GPIOC,ENABLE);
    gpio.GPIO_Pin=GPIO_Pin_6;gpio.GPIO_Mode=GPIO_Mode_AF_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOC,&gpio);
    timer.TIM_Period=100;timer.TIM_Prescaler=4799;TIM_TimeBaseInit(TIM8,&timer);
    pwm.TIM_OCMode=TIM_OCMode_PWM2;pwm.TIM_OutputState=TIM_OutputState_Enable;pwm.TIM_Pulse=compare;pwm.TIM_OCPolarity=TIM_OCPolarity_High;
    TIM_OC1Init(TIM8,&pwm);TIM_CtrlPWMOutputs(TIM8,ENABLE);TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8,ENABLE);TIM_Cmd(TIM8,ENABLE);
}
/* PA1/TIM2 CH2, original PWM2/high polarity. APB1 /2 means the timer
 * clock is HCLK (144 MHz); PSC=143 gives 1 MHz for these short effects. */
static void buzzer_tone(void *context,uint16_t hz)
{
    (void)context;GPIO_InitTypeDef gpio={0};
    TIM_Cmd(TIM2,DISABLE);TIM2->DMAINTENR=0;TIM2->CH2CVR=0;
    gpio.GPIO_Pin=GPIO_Pin_1;gpio.GPIO_Speed=GPIO_Speed_50MHz;
    if(!hz){gpio.GPIO_Mode=GPIO_Mode_Out_PP;GPIO_Init(GPIOA,&gpio);GPIOA->BCR=GPIO_Pin_1;return;}
    gpio.GPIO_Mode=GPIO_Mode_AF_PP;GPIO_Init(GPIOA,&gpio);
    TIM_TimeBaseInitTypeDef timer={0};TIM_OCInitTypeDef pwm={0};
    uint16_t counts=(uint16_t)((1000000u+hz/2)/hz);
    timer.TIM_Period=counts-1;timer.TIM_Prescaler=(uint16_t)(SystemCoreClock/1000000u-1);
    TIM_TimeBaseInit(TIM2,&timer);TIM2->CNT=0;
    pwm.TIM_OCMode=TIM_OCMode_PWM2;pwm.TIM_OutputState=TIM_OutputState_Enable;
    pwm.TIM_Pulse=counts/2;pwm.TIM_OCPolarity=TIM_OCPolarity_High;
    TIM_OC2Init(TIM2,&pwm);TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2,ENABLE);TIM_GenerateEvent(TIM2,TIM_EventSource_Update);TIM_Cmd(TIM2,ENABLE);
}
static void power_off(void) __attribute__((noreturn));
static void power_off(void)
{
    say("DOOM power off; no settings writes\r\n");
    doom_sound_stop(&audio);
    TIM8->CH1CVR=100;GPIOC->BCR=GPIO_Pin_1;
    __asm__ volatile("csrc mstatus,8");for(;;)__asm__ volatile("wfi");
}
void fw_unexpected_trap(void) __attribute__((noreturn));
void fw_unexpected_trap(void)
{__asm__ volatile("csrc mstatus,8");TIM2->CTLR1=0;TIM2->CH2CVR=0;say("DOOM unexpected trap\r\n");GPIOC->BCR=GPIO_Pin_1;for(;;)__asm__ volatile("wfi");}

static void write_screen(void *c,unsigned x,unsigned y,unsigned w,unsigned h,const uint16_t *pixels)
{
    (void)c;
    if(!w || !h || x>=320 || y>=240 || w>320-x || h>240-y)return;
    audio_service();poll_keys(fw_millis());
    display_window(&panel,(uint16_t)x,(uint16_t)y,(uint16_t)w,(uint16_t)h);
    display_write_command(&panel.bus,panel.pixels_command);
    for(unsigned i=0;i<w*h;++i)display_write_data(&panel.bus,pixels[i]);
}
static void init_board(void)
{
    GPIO_InitTypeDef gpio={0};
    SystemCoreClockUpdate();fw_delay_init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
    /* Gate off inherited measurement DMA/UART. No command is sent to ASIC. */
    DMA1_Channel5->CFGR=0;USART1->CTLR1=0;
    /* SPI1 CS inactive, no SPI clock and no transactions. */
    GPIOA->BSHR=GPIO_Pin_4;
    gpio.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_1;gpio.GPIO_Mode=GPIO_Mode_Out_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOA,&gpio);
    GPIOA->BCR=GPIO_Pin_1; /* buzzer off */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);TIM_DeInit(TIM2);
    doom_sound_init(&audio,0,buzzer_tone);
    gpio.GPIO_Pin=GPIO_Pin_1;GPIO_Init(GPIOC,&gpio);GPIOC->BSHR=GPIO_Pin_1;
    gpio.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
    gpio.GPIO_Mode=GPIO_Mode_IPU;GPIO_Init(GPIOC,&gpio);
    /* Bootloader may leave EXTI enabled: this app polls and has no key ISR. */
    EXTI->INTENR=0;
    fw_debug_init(115200);say("POTAT-99 DOOM " DOOM_VERSION "; app 0x6400; SRAM 32K; no storage writes\r\n");
    fw_delay_ms(300);
    gpio.GPIO_Pin=0x20b8;gpio.GPIO_Mode=GPIO_Mode_Out_PP;GPIO_Init(GPIOC,&gpio);GPIOC->BSHR=0x20b8;
    gpio.GPIO_Pin=0xffff;GPIO_Init(GPIOB,&gpio);GPIOB->BSHR=0xffff;
    GPIOC->BSHR=GPIO_Pin_3;fw_delay_ms(10);GPIOC->BCR=GPIO_Pin_3;fw_delay_ms(20);GPIOC->BSHR=GPIO_Pin_3;fw_delay_ms(20);
    display_panel_sequence(&panel,delay,0);brightness(20);
}

/* Exported counters are debugger/emulator observability, never persistent. */
volatile uint32_t doom_frame_count,doom_action_count;
volatile uint32_t doom_sound_events_seen;
static const doom_screen screen={0,write_screen};
static void collect_audio(void)
{
    uint8_t events=doom_take_events();
    if(app.mode!=DOOM_GAME || !app.sound_enabled){if(audio.active || audio.pending)doom_sound_stop(&audio);}
    else if(events){doom_sound_events_seen|=events;doom_sound_events(&audio,events,fw_millis());}
}
static void input(char key)
{bool was_drawing=drawing;drawing=true;doom_app_input(&app,key);drawing=was_drawing;collect_audio();}

static uint8_t buttons_down(void)
{
    static const uint8_t pins[]={10,11,9,12,8,2};
    uint8_t down=0;uint32_t port=GPIOC->INDR;
    for(unsigned i=0;i<6;++i)if(!(port&(1u<<pins[i])))down|=(uint8_t)(1u<<i);
    return down;
}
static int32_t accumulate(int32_t old,int32_t delta)
{int32_t v=old+delta;return v>64*256?64*256:v< -64*256?-64*256:v;}
static void poll_keys(uint32_t now)
{
    doom_control_event e=doom_controls_poll(&controls,buttons_down(),now,app.mode==DOOM_GAME);
    if(e.power)power_off();
    pending_move=accumulate(pending_move,e.move_q8);pending_turn=accumulate(pending_turn,e.turn_q8);
    if(e.key){dispatch(e.key);++doom_action_count;}
}
int main(void)
{
    init_board();doom_app_init(&app);
    doom_controls_init(&controls,buttons_down(),fw_millis());
    uint32_t last_update=fw_millis(),last=last_update-200;
    doom_set_service(game_service);
    for(;;){
        uint32_t now=fw_millis();
        drawing=true;doom_app_update(&app,now-last_update);drawing=false;
        last_update=now;collect_audio();
        poll_keys(fw_millis());drain_keys();audio_service();
        if(app.dirty && now-last>=200){drawing=true;doom_app_draw(&app,&screen);drawing=false;++doom_frame_count;last=now;}
        fw_delay_ms(5);
    }
}

__attribute__((section(".fw_info"),used)) static const struct {
    char magic[8];uint32_t layout,load,settings;char version[24];
} info={"DMT99FW1",1,0x08006400,0x08040000,DOOM_VERSION_TEXT};
