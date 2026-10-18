#ifndef DMT_FILESYSTEM_H
#define DMT_FILESYSTEM_H
#include <stdint.h>
#include "../vendor/fatfs/ff.h"
/* Single foreground FatFs instance, matching the non-reentrant original.
 * No callbacks means no disk access. A missing write callback protects evidence. */
typedef struct {
 void *context;
 int (*read)(void *,uint32_t byte_offset,void *,uint32_t bytes);
 int (*write)(void *,uint32_t byte_offset,const void *,uint32_t bytes);
 void (*unresolved)(void *,uint32_t original_entry);
} dmt_filesystem_io;
void filesystem_bind(const dmt_filesystem_io *);
FRESULT filesystem_mount(FATFS *,unsigned volume);
/* Raw chip read through the bound device (0 = ok); used by the theme installer
 * to read a package on volume 0 while volume 1 is mounted. */
int filesystem_raw_read(uint32_t byte_offset,void *out,uint32_t bytes);
/* Original dd4e startup partition rewrite, explicit and write-capability gated.
 * Not called by offline main or ordinary resource mounting. */
FRESULT filesystem_original_partition_setup(void *sector_work);
#endif
