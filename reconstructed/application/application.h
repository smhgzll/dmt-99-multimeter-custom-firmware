#ifndef DMT_APPLICATION_H
#define DMT_APPLICATION_H
#include "control.h"
#include "last_mode.h"
#include "reading.h"
#include "screen.h"
#include "menu.h"
#include "power.h"
#include "persistence.h"
#include "history.h"
#include "history_browser.h"
#include "thresholds.h"
#include "status_view.h"
#include "config.h"
#if DMT_THEMES
#include "theme_ui.h"
#endif
#if DMT_SIMPLE_SCREEN
#include "simple_screen.h"
#endif
enum dmt_run_state {DMT_RUNNING,DMT_MENU,DMT_EDIT_THRESHOLD,DMT_LANGUAGE_SELECT,DMT_BROWSE_HISTORY,DMT_CHARGING,DMT_STOPPED,DMT_DIAGNOSTIC,DMT_THEME_MANAGER};
typedef struct {
    dmt_control control;dmt_menu menu;dmt_power power;
    dmt_settings_image settings;dmt_history history;dmt_history_browser browser;dmt_threshold_editor editor;
    enum dmt_run_state run;
    uint8_t frame[16],saved_event[16];
    uint8_t rx_suppressed,event_consumed,last_ui_event;
    uint8_t language_choice,history_row,history_wrapped,alarm,menu_origin_view;
    char previous_history_text[16];
    uint8_t battery_percent,settings_loaded;
    uint32_t unresolved_calls,last_unresolved_address;
#if DMT_THEMES
    dmt_theme_menu themes;
#endif
#if DMT_SIMPLE_SCREEN
    dmt_simple_cache simple;
#endif
} dmt_application;
typedef struct {
    void *context;
    void (*send)(void *,const uint8_t[10]);
    bool (*longpress)(void *,uint8_t);
    void (*delay_ms)(void *,uint32_t);
    bool (*charging)(void *);
    uint8_t (*battery_read)(void *);
    void (*buzzer)(void *,bool);
    void (*backlight)(void *,uint8_t compare);
    void (*power_latch)(void *,bool);
    void (*effect)(void *,enum dmt_effect,const dmt_control *);
    void (*menu_action)(void *,enum dmt_menu_action,uint8_t,const dmt_menu *,const dmt_control *);
    /* Required boundary for remaining source gaps. Return false if unsupported;
     * never emulate an unknown routine with success. Address is original PC. */
    bool (*unresolved)(void *,uint32_t original_address,const dmt_application *);
    const dmt_settings_backend *internal_flash;
    const dmt_canvas *canvas;
    const dmt_status_view *status_view;
    dmt_reading *reading;
    const dmt_view *view;
    const dmt_last_mode_backend *last_mode;
    dmt_screen *screen;
    const dmt_button_io *diagnostic_buttons;
    /* Signed 75f0 result used by the TIM3 low-battery test (original compares
     * the int, so readings below 3400 mV are negative and shut down). Falls
     * back to battery_read, whose byte cannot represent negative levels. */
    int (*battery_level)(void *);
#if DMT_THEMES
    const dmt_theme_io *themes;     /* theme manager (About page, ENTER) */
#endif
} dmt_application_io;
void application_init(dmt_application *,const dmt_settings_image *,const dmt_application_io *);
/* Explicit8ed4 boot-key entry. Normal callers retain application_init behavior.
 * No diagnostic command is sent unless both PC10/PC11 inputs are low. */
bool application_boot(dmt_application *,const dmt_settings_image *,const dmt_application_io *);
void application_step(dmt_application *,const dmt_application_io *);
/* ISR callers serialize updates with the foreground. On target use IRQ masking
 * around the 16-byte snapshot; these pure entry points perform no MMIO. */
void application_receive_idle(dmt_application *,const uint8_t[13]);
void application_timer(dmt_application *,const dmt_application_io *);
void application_power_down(dmt_application *,const dmt_application_io *);
void application_beep_500(const dmt_application_io *); /* 792e */
void application_reset_history_strip(dmt_application *,const dmt_application_io *);
void application_invalidate_display(dmt_application *,const dmt_application_io *);
/* 908a: exact startup call order. These typed interfaces identify all original
 * callees; the offline executable never binds them to MMIO. loop is a
 * nonreturning target boundary, permitted to return in a host call-trace test. */
typedef struct {
    void *context;
    void (*priority_group)(void *,uint32_t); /* cb8e */
    void (*clock_update)(void *);           /* d2fc */
    void (*delay_init)(void *);             /* 1a626 */
    void (*debug_uart)(void *,uint32_t);    /* 1a682 */
    void (*measurement_uart)(void *,uint32_t); /* c350 */
    void (*buttons)(void *);                /* 7944 */
    void (*battery_inputs)(void *);         /* 770a */
    void (*display)(void *);                /* 87aa */
    void (*resources)(void *);              /* dd4e */
    void (*timer)(void *,uint16_t,uint16_t); /* 7736 */
    void (*loop)(void *);                   /* 8ed4 */
} dmt_startup_io;
bool application_startup(const dmt_startup_io *);
#endif
