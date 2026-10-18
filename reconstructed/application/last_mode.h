#ifndef DMT_LAST_MODE_H
#define DMT_LAST_MODE_H
#include "config.h"
#include "control.h"
/* Independent 16-byte record of the last acknowledged measurement mode, kept
 * outside the V2.4 settings page. Active with DMT_LAST_MODE (CUSTOM_DMT99 and
 * release builds). write() may only update a RAM copy; commit(), when present,
 * persists it and is called at power-down next to the settings commit, so a
 * mode change costs no Flash cycle (firmware/last_mode_flash.c). */
typedef struct {void *context;bool (*read)(void *,uint8_t bytes[16]);bool (*write)(void *,const uint8_t bytes[16]);bool (*commit)(void *);} dmt_last_mode_backend;
bool last_mode_valid(uint8_t view,uint8_t mode);
bool last_mode_load(const dmt_last_mode_backend *,uint8_t view,uint8_t *mode);
bool last_mode_save(const dmt_last_mode_backend *,uint8_t view,uint8_t mode);
bool last_mode_forget(const dmt_last_mode_backend *);
bool last_mode_restore(dmt_control *,const dmt_last_mode_backend *,void (*send)(void *,const uint8_t[10]),void *);
bool last_mode_commit(const dmt_last_mode_backend *);
#endif
