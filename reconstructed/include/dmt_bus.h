#ifndef DMT_BUS_H
#define DMT_BUS_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {
    void *context;
    void (*select)(void *,bool active);
    uint8_t (*transfer)(void *,uint8_t value);
} dmt_spi_bus;
typedef struct {
    void *context;
    void (*set_pc)(void *,uint16_t mask,bool high);
    void (*write_pb)(void *,uint16_t value);
} dmt_display_bus;
#endif
