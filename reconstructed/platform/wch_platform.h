#ifndef DMT_WCH_PLATFORM_H
#define DMT_WCH_PLATFORM_H
#include "ch32v30x.h"
#include "measurement_asic.h"
/* wch_nvic.c: original NVIC_PriorityGroupConfig/NVIC_Init (cb8e/cb94). */
void recovered_nvic_priority_group(uint32_t group);
uint8_t recovered_nvic_priority_byte(uint32_t group,uint8_t preemption,uint8_t sub);
void recovered_nvic_init(const NVIC_InitTypeDef *);
void recovered_measurement_uart_init(uint32_t baud,uint8_t dma_buffer[13]);
void recovered_spi1_init(void);
void recovered_measurement_send(const uint8_t command[10]);
bool recovered_asic_transmit(void *context,const uint8_t command[10]);
#endif
