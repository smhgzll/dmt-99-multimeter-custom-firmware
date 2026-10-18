#ifndef DMT_THEME_H
#define DMT_THEME_H
/* Installable themes on the assets volume (release builds, DMT_THEMES).
 *
 * A theme is an overlay: themes/Tnn/<asset path> holds replacements for some
 * original assets; THEME.INF (written last) makes the slot valid. The
 * original files are never modified. While a theme is active, an asset is
 * looked up in the theme folder first when the theme's Bloom filter says it
 * may be there, and the original is used otherwise.
 *
 * THEME.PKG (DMT99 Studio export, copied to the bootloader's USB disk):
 *   0  "DMT99THM"  8  version u16 = 1  10 file count u16  12 total size u32
 *   16 name[24]    40 directory offset u32   44 data offset u32
 *   directory: { offset u32, size u32, crc32 u32, path_len u8, path }
 *   data, then crc32 of every preceding byte (last 4 bytes).
 * Paths are assets-volume paths of existing original .jpg files
 * ("mode/dc_v.jpg", characters A-Z a-z 0-9 _ - . ~ /); every file must be a JPEG. One optional entry named
 * THEME.PAL holds up to 16 colour pairs (from u16, to u16; RGB565) that
 * recolour what the firmware draws itself (classic digits, menu text,
 * borders); it is stored in THEME.INF, not as a file. */
#include "../storage/filesystem.h"
#include <stdbool.h>
#include <stdint.h>

#define DMT_THEME_SLOTS 99u
#define DMT_THEME_LIST_MAX 16u
#define DMT_THEME_NAME 24u
#define DMT_THEME_PALETTE 16u
#define DMT_THEME_SETTINGS_INDEX 118u  /* settings halfwords 118 (slot) and 119 (slot ^ 0xa5a5) */

typedef struct {
    uint8_t slot;          /* 0 = original assets */
    uint8_t palette_count;
    uint8_t bloom[64];
    uint16_t palette[DMT_THEME_PALETTE][2];
} dmt_theme_active;

typedef struct {
    uint8_t slot;
    bool valid;            /* THEME.INF present and consistent */
    char name[DMT_THEME_NAME + 1];
    uint16_t files;
    uint32_t bytes;
} dmt_theme_info;

typedef struct {
    void *context;
    bool (*read)(void *, uint32_t offset, uint8_t *out, uint32_t n);
    uint32_t size;
} dmt_theme_source;

typedef struct {
    void *context;
    void (*progress)(void *, const char *what, uint32_t done, uint32_t total);
} dmt_theme_progress;

enum dmt_theme_result {
    DMT_THEME_OK = 0,
    DMT_THEME_E_FORMAT,    /* not a theme package / unsupported version */
    DMT_THEME_E_CRC,       /* package damaged */
    DMT_THEME_E_PATH,      /* a file does not replace an existing original JPEG */
    DMT_THEME_E_SPACE,     /* not enough free space on the assets volume */
    DMT_THEME_E_FULL,      /* no free slot */
    DMT_THEME_E_IO,        /* file system error */
    DMT_THEME_E_VERIFY,    /* written file did not read back */
};

uint32_t theme_hash(const char *path);
/* Theme-relative path for an asset path if the active theme may replace it. */
bool theme_path(const dmt_theme_active *, const char *asset, char *out, unsigned n);
/* Colour of a firmware-drawn primitive under the active theme's palette. */
uint16_t theme_color(const dmt_theme_active *, uint16_t rgb565);
bool theme_info(uint8_t slot, dmt_theme_info *, FIL *scratch);
bool theme_activate(dmt_theme_active *, uint8_t slot, FIL *scratch);
unsigned theme_list(uint8_t *slots, unsigned max);
uint32_t theme_free_bytes(void);
/* Removes every file and folder of a slot (valid or incomplete). */
enum dmt_theme_result theme_remove(uint8_t slot, const dmt_theme_progress *);
/* Validates the whole package, copies it into a new slot and verifies it. */
enum dmt_theme_result theme_install(const dmt_theme_source *, FIL *scratch, uint8_t *buffer, uint32_t buffer_size,
                                    const dmt_theme_progress *, uint8_t *slot_out, char name_out[DMT_THEME_NAME + 1]);
const char *theme_result_text(enum dmt_theme_result);
/* Settings halfwords 118/119 <-> slot. */
uint8_t theme_settings_slot(const uint16_t *halfwords);
void theme_settings_store(uint16_t *halfwords, uint8_t slot);
#endif
