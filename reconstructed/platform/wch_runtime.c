/* Official-SDK translations of the original peripheral set-up routines:
 * 7944 (keys, power latch), 7716/7736 (TIM3 1 s tick), 780c/78ba/78d0 (TIM2
 * buzzer), 7f66 (TIM8 backlight) and the GPIO/reset part of 87aa (panel bus).
 * Call order and SDK arguments follow the original instructions. Interrupt
 * handlers live with the target firmware (firmware/isr.c).
 */
#include "wch_runtime.h"
#include "wch_platform.h"

void recovered_buttons_init(void (*delay)(void *,uint32_t),void *ctx)
{
    GPIO_InitTypeDef gpio={0};EXTI_InitTypeDef exti={0};NVIC_InitTypeDef irq={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    /* PC1 drives the power latch; asserted 300 ms after reset. */
    gpio.GPIO_Pin=GPIO_Pin_1;gpio.GPIO_Speed=GPIO_Speed_50MHz;gpio.GPIO_Mode=GPIO_Mode_Out_PP;GPIO_Init(GPIOC,&gpio);
    delay(ctx,300);GPIO_WriteBit(GPIOC,GPIO_Pin_1,Bit_SET);
    /* Order, EXTI lines and PFIC priorities exactly as 7944. */
    static const uint8_t pins[]={10,11,9,12,8,2};
    static const uint8_t channel[]={EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI9_5_IRQn,EXTI15_10_IRQn,EXTI9_5_IRQn,EXTI2_IRQn};
    static const uint8_t sub[]={1,1,1,1,2,1};
    for(unsigned i=0;i<6;++i){
        gpio.GPIO_Pin=(uint16_t)(1u<<pins[i]);gpio.GPIO_Mode=GPIO_Mode_IPU;GPIO_Init(GPIOC,&gpio);
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,pins[i]);
        exti.EXTI_Line=gpio.GPIO_Pin;exti.EXTI_Mode=EXTI_Mode_Interrupt;exti.EXTI_Trigger=EXTI_Trigger_Falling;exti.EXTI_LineCmd=ENABLE;EXTI_Init(&exti);
        irq.NVIC_IRQChannel=channel[i];irq.NVIC_IRQChannelPreemptionPriority=1;irq.NVIC_IRQChannelSubPriority=sub[i];irq.NVIC_IRQChannelCmd=ENABLE;recovered_nvic_init(&irq);
    }
}
void recovered_periodic_timer_init(uint16_t period,uint16_t prescaler)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    NVIC_InitTypeDef irq={0};
    irq.NVIC_IRQChannel=TIM3_IRQn;irq.NVIC_IRQChannelPreemptionPriority=3;irq.NVIC_IRQChannelSubPriority=3;irq.NVIC_IRQChannelCmd=ENABLE;recovered_nvic_init(&irq);
    TIM_TimeBaseInitTypeDef timer={0};timer.TIM_Period=period;timer.TIM_Prescaler=prescaler;timer.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&timer);TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);TIM_Cmd(TIM3,ENABLE);
}
void recovered_display_gpio_init(void (*delay)(void *,uint32_t),void *ctx)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
    /* PC3 reset, PC4 CS, PC5 D/C, PC7 (held high, presumably RD), PC13 WR. */
    GPIO_InitTypeDef gpio={0};gpio.GPIO_Pin=0x20b8;gpio.GPIO_Mode=GPIO_Mode_Out_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOC,&gpio);GPIO_SetBits(GPIOC,0x20b8);
    gpio.GPIO_Pin=0xffff;GPIO_Init(GPIOB,&gpio);GPIO_SetBits(GPIOB,0xffff);
    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_SET);delay(ctx,10);
    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET);delay(ctx,20);
    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_SET);delay(ctx,20);
}
void recovered_display_set_pc(void *ctx,uint16_t mask,bool high){(void)ctx;if(high)GPIOC->BSHR=mask;else GPIOC->BCR=mask;}
void recovered_display_write_pb(void *ctx,uint16_t value){(void)ctx;GPIOB->OUTDR=value;}
void recovered_backlight_pwm(uint16_t period,uint16_t prescaler,uint16_t compare)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO_InitTypeDef gpio={0};gpio.GPIO_Pin=GPIO_Pin_6;gpio.GPIO_Mode=GPIO_Mode_AF_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOC,&gpio);
    TIM_TimeBaseInitTypeDef timer={0};timer.TIM_Period=period;timer.TIM_Prescaler=prescaler;TIM_TimeBaseInit(TIM8,&timer);
    TIM_OCInitTypeDef pwm={0};pwm.TIM_OCMode=TIM_OCMode_PWM2;pwm.TIM_OutputState=TIM_OutputState_Enable;pwm.TIM_Pulse=compare;pwm.TIM_OCPolarity=TIM_OCPolarity_High;
    TIM_OC1Init(TIM8,&pwm);TIM_CtrlPWMOutputs(TIM8,ENABLE);TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);TIM_ARRPreloadConfig(TIM8,ENABLE);TIM_Cmd(TIM8,ENABLE);
}
void recovered_backlight_compare(uint8_t compare){recovered_backlight_pwm(100,4799,compare);}
static void buzzer_pwm(uint16_t period,uint16_t prescaler,uint16_t compare)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef gpio={0};gpio.GPIO_Pin=GPIO_Pin_1;gpio.GPIO_Mode=GPIO_Mode_AF_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOA,&gpio);
    TIM_TimeBaseInitTypeDef timer={0};timer.TIM_Period=period;timer.TIM_Prescaler=prescaler;TIM_TimeBaseInit(TIM2,&timer);
    TIM_OCInitTypeDef pwm={0};pwm.TIM_OCMode=TIM_OCMode_PWM2;pwm.TIM_OutputState=TIM_OutputState_Enable;pwm.TIM_Pulse=compare;pwm.TIM_OCPolarity=TIM_OCPolarity_High;
    TIM_OC2Init(TIM2,&pwm);TIM_CtrlPWMOutputs(TIM2,ENABLE);TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);TIM_ARRPreloadConfig(TIM2,ENABLE);TIM_Cmd(TIM2,ENABLE);
}
void recovered_buzzer_enable(bool enabled)
{
    if(enabled){buzzer_pwm(383,99,191);return;}  /* 78ba: ~3.75 kHz, 50 % */
    TIM_SetCompare2(TIM2,0);
    GPIO_InitTypeDef gpio={0};gpio.GPIO_Pin=GPIO_Pin_1;gpio.GPIO_Mode=GPIO_Mode_Out_PP;gpio.GPIO_Speed=GPIO_Speed_50MHz;GPIO_Init(GPIOA,&gpio);
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}
