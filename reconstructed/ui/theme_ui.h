#ifndef DMT_THEME_UI_H
#define DMT_THEME_UI_H
/* Theme startup (install a pending 0:/THEME.PKG, activate the selected slot)
 * and the theme manager screen opened from the About page. */
#include "theme.h"
#include "graphics.h"
#include "simple_screen.h"

/* Operations the theme manager needs from its host (firmware or simulator). */
typedef struct {
    void *context;
    unsigned (*list)(void *, uint8_t *slots, unsigned max);
    bool (*info)(void *, uint8_t slot, dmt_theme_info *);
    uint8_t (*active)(void *);
    /* Activates a slot (0 = original) and stores it in the RAM settings image. */
    bool (*select)(void *, uint8_t slot);
    enum dmt_theme_result (*remove)(void *, uint8_t slot, const dmt_theme_progress *);
    uint32_t (*free_bytes)(void *);
    /* Simple screen settings (RAM settings image, stored at power-off). */
    void (*simple_get)(void *, dmt_simple_config *);
    void (*simple_set)(void *, const dmt_simple_config *);
} dmt_theme_io;

typedef struct {
    uint8_t count, cursor, top;              /* rows: SIMPLE_FIELDS settings, then themes */
    bool confirm;
    uint8_t slots[DMT_THEME_LIST_MAX + 1];   /* [0] = 0: original assets */
} dmt_theme_menu;

typedef struct {
    FATFS *fs;
    FIL *scratch;
    uint8_t *buffer;
    uint32_t buffer_size;
    const dmt_canvas *canvas;
    void (*delay_ms)(void *, uint32_t);
    void *context;
} dmt_theme_boot;

/* Returns the slot to use: a newly installed theme, else `selected` if it is
 * still valid, else 0. `installed` reports whether a package was processed. */
uint8_t theme_boot(const dmt_theme_boot *, uint8_t selected, dmt_theme_active *, bool *installed,
                   enum dmt_theme_result *result);

void theme_menu_open(dmt_theme_menu *, const dmt_theme_io *, const dmt_canvas *);
/* Handles one key event. Setting rows: 5 next value, 3 previous value. Theme
 * rows: 5 use, 3 delete (asks, 5 confirms). 1/2 move, 4/6 leave.
 * Returns true when the user leaves the theme manager. */
bool theme_menu_step(dmt_theme_menu *, uint8_t *event, const dmt_theme_io *, const dmt_canvas *);
/* About page hint (release builds). */
void theme_about_hint(const dmt_canvas *);
#endif
