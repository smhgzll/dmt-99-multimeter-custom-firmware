#ifndef DMT_STORAGE_PORT_H
#define DMT_STORAGE_PORT_H
#include "dmt_bus.h"
#include "filesystem.h"
typedef struct {
    uint32_t jedec,capacity;
    FRESULT fdisk,mount0,label,mount1;
    uint32_t writes_skipped,sectors_written,writes_refused,write_errors;
    uint32_t codepage_faults,last_codepage_entry;
} fw_storage_status;
extern const dmt_spi_bus fw_spi_bus;
extern fw_storage_status fw_storage;
void fw_spi_flash_init(void);                 /* d86c: d38a + d782 */
void fw_storage_bind(void);
void fw_resources_init(FATFS *,void *work);   /* dd4e; work >= 4096 bytes */
#endif
