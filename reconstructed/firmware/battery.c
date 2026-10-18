/* Battery and charger inputs: 770a, 767a, 7562, 7598, 75f0.
 *
 * 767a: ADC1 clock = PCLK2/8 (18 MHz at 144 MHz, as the original), PC0 analog,
 * independent single software-triggered conversion, right aligned, then reset
 * calibration and calibration, then one software start. The original passes
 * uninitialized ADC_OutputBuffer/ADC_Pga words to ADC_Init; tracing original
 * main from 0x908a shows they hold 0xc and 0x1 left by the PC2 EXTI_Init
 * structure, which only reach CTLR1.AWDCH (analog watchdog channel, unused).
 * They are reproduced, so ADC1 CTLR1 equals the original value 0xd.
 * 7598: per sample, configure rank 1 with 239.5 cycles, software start, poll
 * EOC, sum, delay 5 ms; return the truncated mean.
 * 75f0: arithmetic in application/battery_level.c; the original prints both
 * values on the debug console ("voltage:%f" and GBK "电量：%f").
 */
#include "ch32v30x.h"
#include "firmware.h"
#include "battery_level.h"

static void charge_detect_gpio_init(void)
{
    GPIO_InitTypeDef gpio={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    gpio.GPIO_Pin=GPIO_Pin_8;gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;gpio.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio);
}
static void adc_battery_init(void)
{
    GPIO_InitTypeDef gpio={0};ADC_InitTypeDef adc={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC,ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    gpio.GPIO_Pin=GPIO_Pin_0;gpio.GPIO_Speed=GPIO_Speed_50MHz;gpio.GPIO_Mode=GPIO_Mode_AIN;
    GPIO_Init(GPIOC,&gpio);
    adc.ADC_Mode=ADC_Mode_Independent;adc.ADC_ScanConvMode=DISABLE;adc.ADC_ContinuousConvMode=DISABLE;
    adc.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;adc.ADC_DataAlign=ADC_DataAlign_Right;
    /* Original leftover words 0xc|0x1 land in CTLR1.AWDCH (=13); reproduced
     * so CTLR1 matches the original register value. AWDEN stays clear. */
    adc.ADC_NbrOfChannel=1;adc.ADC_OutputBuffer=0xc;adc.ADC_Pga=0x1;
    ADC_Init(ADC1,&adc);
    ADC_Cmd(ADC1,ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1)){}
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)){}
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
void fw_battery_inputs_init(void){adc_battery_init();charge_detect_gpio_init();}
bool fw_charging(void){return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)!=Bit_RESET;}
uint16_t fw_adc_read_averaged(uint8_t channel,uint8_t samples)
{
    uint32_t sum=0;uint8_t n=0;
    ADC_RegularChannelConfig(ADC1,channel,1,ADC_SampleTime_239Cycles5);
    for(;n!=samples;++n){
        ADC_SoftwareStartConvCmd(ADC1,ENABLE);
        /* Bounded where the original polls EOC forever; a stuck ADC reads as 0. */
        uint32_t polls=0;
        while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET){if(++polls>1000000u)return 0;}
        sum+=ADC_GetConversionValue(ADC1);
        fw_delay_ms(5);
    }
    return n?(uint16_t)(sum/n):0;
}
int fw_battery_percent(void)
{
    float millivolts,level;
    int percent=battery_level_percent(fw_adc_read_averaged(ADC_Channel_10,50),&millivolts,&level);
    fw_printf("voltage:%f\r\n",(double)millivolts);
    fw_printf("\xb5\xe7\xc1\xbf\xa3\xba%f\r\n",(double)level);
    return percent;
}
