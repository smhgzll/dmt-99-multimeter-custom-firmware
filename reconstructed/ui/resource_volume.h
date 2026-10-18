#ifndef DMT_RESOURCE_VOLUME_H
#define DMT_RESOURCE_VOLUME_H
#include "resources.h"
/* Shared mount object and last status preserve ddB0/de12 remount ordering. */
bool resource_optional_logo(dmt_resources *,FATFS *,FRESULT *last_status);
void resource_remove_logo(FATFS *,FRESULT *last_status);
#endif
