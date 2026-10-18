#ifndef DMT_CODEPAGE_H
#define DMT_CODEPAGE_H
#include <stdint.h>
typedef enum {DMT_CODEPAGE_DEFINED,DMT_CODEPAGE_UNMAPPED_ROM} dmt_codepage_status;
typedef struct {uint16_t value;dmt_codepage_status status;uint32_t fault_address;uint8_t reads;} dmt_codepage_result;
dmt_codepage_result codepage_stock_convert(uint32_t code,uint16_t page,int to_oem);
uint32_t filesystem_codepage_fault(void);
#endif
