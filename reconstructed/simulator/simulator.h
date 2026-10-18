#ifndef DMT_SIMULATOR_H
#define DMT_SIMULATOR_H
#include "application.h"
#include "resources.h"
#include "resource_volume.h"
#include "asic_driver.h"
#include "config.h"
#if DMT_THEMES
#include "theme_ui.h"
#endif
#include <stdio.h>
enum {DMT_SIM_WIDTH=320,DMT_SIM_HEIGHT=240};
typedef struct {
 dmt_application app;
 dmt_application_io io;
 dmt_screen screen;
 dmt_canvas canvas;
 dmt_status_view status;
 dmt_resources resources;
 FATFS filesystem;
 dmt_settings_backend settings_backend;
 dmt_last_mode_backend last_backend;
 dmt_asic_io asic;
 dmt_button_io diagnostic_buttons;
 dmt_settings_image stored_settings;
 uint8_t last_record[16],tx[10];
 uint16_t pixels[DMT_SIM_WIDTH*DMT_SIM_HEIGHT];
 uint8_t *flash;
 uint32_t tx_count,settings_writes,last_reads,last_writes,resource_errors,external_gaps,clipped_pixels;
 uint64_t elapsed_ms;
 uint8_t battery,backlight;
 uint16_t pressed_pins;
 uint32_t rejected_tx;
 uint32_t packets; /* display/event packets delivered (simulated device frame counter) */
 bool charging,buzzer,power,longpress;
 /* Every accepted UART command, oldest first (host diagnostics only). */
 uint8_t tx_log[64][10];
 uint32_t tx_logged;
 /* Optional JSON-lines draw trace (host tooling: DMT99 Studio screen map). */
 FILE *draw_trace;
 bool writable;          /* FatFs writes go to the in-memory flash copy */
#if DMT_THEMES
 dmt_theme_active theme;
 dmt_theme_io theme_io;
 uint8_t theme_slot;
 /* --palette: preview colour pairs that replace the theme's palette. */
 uint8_t preview_palette_count;
 uint16_t preview_palette[DMT_THEME_PALETTE][2];
 dmt_canvas jpeg_canvas;   /* JPEG output is not recoloured, as on the meter */
#endif
} dmt_simulator;
bool simulator_open(dmt_simulator *,const char *flash_path);
void simulator_close(dmt_simulator *);
void simulator_power_on(dmt_simulator *);
bool simulator_diagnostic_boot(dmt_simulator *);
/* Fixture configuration is distinct from recovered menu/button input. */
bool simulator_configure(dmt_simulator *,unsigned view,unsigned layout,unsigned english);
bool simulator_button(dmt_simulator *,unsigned event,bool longpress);
void simulator_packet(dmt_simulator *,const uint8_t[13]);
void simulator_ack(dmt_simulator *,uint8_t mode);
bool simulator_reading(dmt_simulator *,const char *four_digits,unsigned unit);
bool simulator_snapshot(dmt_simulator *,const char *path);
/* Draw trace: one JSON object per JPEG/text/bitmap/fill call; 0 stops it. */
bool simulator_trace(dmt_simulator *,const char *path);
/* Allow FatFs writes (theme install/delete, history files) to the memory copy. */
void simulator_writable(dmt_simulator *);
/* Writes the in-memory flash to a new file (never the image it was opened from). */
bool simulator_flash_save(const dmt_simulator *,const char *path,const char *source);
void simulator_trace_marker(dmt_simulator *,const char *kind,const char *name);
/* Optional process persistence is a simulator-owned file, never an acquired
 * image, and contains a separate custom record. Existing non-state files fail. */
bool simulator_state_load(dmt_simulator *,const char *path);
bool simulator_state_save(const dmt_simulator *,const char *path);
#endif
