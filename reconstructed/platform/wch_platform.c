/* SDK-based translations of 0xc350,0x97c4,0xd38a. Bound by the target firmware
 * (firmware/main.c); never called by the offline harness or host tests. */
#include "wch_platform.h"
#include "asic_driver.h"
void recovered_measurement_uart_init(uint32_t baud,uint8_t rx[13])
{
    GPIO_InitTypeDef gpio={0};USART_InitTypeDef uart={0};DMA_InitTypeDef dma={0};NVIC_InitTypeDef irq={0};
    RCC_APB2PeriphClockCmd(0x4004,ENABLE);
    gpio.GPIO_Pin=GPIO_Pin_9;gpio.GPIO_Speed=GPIO_Speed_50MHz;gpio.GPIO_Mode=GPIO_Mode_AF_PP;GPIO_Init(GPIOA,&gpio);
    gpio.GPIO_Pin=GPIO_Pin_10;gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;GPIO_Init(GPIOA,&gpio);
    uart.USART_BaudRate=baud;uart.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;USART_Init(USART1,&uart);
    irq.NVIC_IRQChannel=USART1_IRQn;irq.NVIC_IRQChannelPreemptionPriority=2;
    irq.NVIC_IRQChannelSubPriority=2;irq.NVIC_IRQChannelCmd=ENABLE;recovered_nvic_init(&irq);
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);USART_Cmd(USART1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    dma.DMA_PeripheralBaseAddr=(uint32_t)(uintptr_t)&USART1->DATAR;dma.DMA_MemoryBaseAddr=(uint32_t)(uintptr_t)rx;
    dma.DMA_BufferSize=13;dma.DMA_MemoryInc=DMA_MemoryInc_Enable;dma.DMA_Mode=DMA_Mode_Circular;dma.DMA_Priority=DMA_Priority_High;
    DMA_Init(DMA1_Channel5,&dma);
    irq.NVIC_IRQChannel=DMA1_Channel5_IRQn;irq.NVIC_IRQChannelPreemptionPriority=1;irq.NVIC_IRQChannelSubPriority=1;recovered_nvic_init(&irq);
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);DMA_Cmd(DMA1_Channel5,ENABLE);USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
}
void recovered_measurement_send(const uint8_t command[10])
{
    for(unsigned i=0;i<10;++i){USART_SendData(USART1,command[i]);while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET){}}
}
bool recovered_asic_transmit(void *context,const uint8_t command[10])
{
    (void)context;
    if(asic_classify_tx(command,10)==DMT_ASIC_TX_INVALID)return false;
    recovered_measurement_send(command);return true;
}
void recovered_spi1_init(void)
{
    GPIO_InitTypeDef gpio={0};SPI_InitTypeDef spi={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA,ENABLE);
    gpio.GPIO_Speed=GPIO_Speed_50MHz;gpio.GPIO_Pin=GPIO_Pin_4;gpio.GPIO_Mode=GPIO_Mode_Out_PP;GPIO_Init(GPIOA,&gpio);GPIO_SetBits(GPIOA,GPIO_Pin_4);
    gpio.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_7;gpio.GPIO_Mode=GPIO_Mode_AF_PP;GPIO_Init(GPIOA,&gpio);
    gpio.GPIO_Pin=GPIO_Pin_6;gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;GPIO_Init(GPIOA,&gpio);
    spi.SPI_Direction=SPI_Direction_2Lines_FullDuplex;spi.SPI_Mode=SPI_Mode_Master;spi.SPI_DataSize=SPI_DataSize_8b;
    spi.SPI_CPOL=SPI_CPOL_High;spi.SPI_CPHA=SPI_CPHA_2Edge;spi.SPI_NSS=SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;spi.SPI_FirstBit=SPI_FirstBit_MSB;spi.SPI_CRCPolynomial=7;
    SPI_Init(SPI1,&spi);SPI_Cmd(SPI1,ENABLE);
}
