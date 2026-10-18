/* Build identity. DMT_FW_VERSION/DMT_FW_GIT are passed by the Makefile; the
 * record in .fw_info lets host tools identify an image without symbols. */
#include "firmware.h"
#include "config.h"
#ifndef DMT_FW_VERSION
#define DMT_FW_VERSION "0.0-dev"
#endif
#ifndef DMT_FW_GIT
#define DMT_FW_GIT "unknown"
#endif
/* Code Flash address the BIN is written to: 0x08000000 standalone, 0x08006400
 * for the USB update image installed by the manufacturer bootloader. */
#ifndef DMT_FW_LOAD_ADDRESS
#define DMT_FW_LOAD_ADDRESS 0x08000000u
#endif
const char fw_version_text[]="DMT99 reconstructed firmware " DMT_FW_VERSION " (" DMT_BUILD_NAME ", " DMT_FW_GIT ")";
__attribute__((section(".fw_info"),used)) static const struct {
    char magic[8];uint32_t layout_version,load_address,settings_address;char version[24];
} info={{'D','M','T','9','9','F','W','1'},1,DMT_FW_LOAD_ADDRESS,0x08040000u,DMT_FW_VERSION};
