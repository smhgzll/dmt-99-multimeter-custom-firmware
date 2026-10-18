#ifndef DMT_DISK_H
#define DMT_DISK_H
#include "spi_flash.h"
typedef struct {const dmt_spi_bus *bus;uint8_t *sector_scratch;uint32_t poll_budget;} dmt_disk;
int disk_read_recovered(const dmt_disk *,uint8_t drive,uint8_t *,uint32_t sector,uint32_t count);
int disk_write_recovered(const dmt_disk *,uint8_t drive,const uint8_t *,uint32_t sector,uint32_t count);
int disk_ioctl_recovered(uint8_t drive,uint8_t command,void *);
#endif
