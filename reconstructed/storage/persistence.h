#ifndef DMT_PERSISTENCE_H
#define DMT_PERSISTENCE_H
#include <stdbool.h>
#include <stdint.h>
enum {DMT_SETTINGS_BYTES=256,DMT_SETTINGS_ADDRESS=0x08040000};
typedef struct {uint16_t halfwords[128];} dmt_settings_image;
/* Unbound device-specific backend. Never supplied by the offline executable.
 * begin corresponds to 0xd87c clock/CSR/FLASH unlock and flag clear; erase to
 * FLASH_ErasePage_Fast; program returns original FLASH_Status (4=complete).
 */
typedef struct {
 void *context;
 bool (*read)(void *,uint32_t,uint16_t *,uint16_t);
 bool (*begin)(void *);
 bool (*erase_page)(void *,uint32_t);
 uint32_t (*program_halfword)(void *,uint32_t,uint16_t);
 void (*end)(void *);
} dmt_settings_backend;
bool settings_image_load(dmt_settings_image *,const dmt_settings_backend *);
uint32_t settings_image_commit(const dmt_settings_image *,const dmt_settings_backend *,uint32_t initial_flash_status);
void settings_image_defaults(dmt_settings_image *);
/* dc46 external logo-removal result is deliberately ignored as in the BIN. */
void settings_image_restore_defaults(dmt_settings_image *,uint8_t *loaded,void *,void (*logo_remove)(void *));
uint16_t *settings_image_word(dmt_settings_image *);
#endif
