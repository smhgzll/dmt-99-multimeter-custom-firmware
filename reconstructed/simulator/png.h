#ifndef DMT_PNG_H
#define DMT_PNG_H
#include <stdbool.h>
#include <stdint.h>
/* Host-only PNG export of an RGB565 canvas. Deterministic RGB8 PNG, filter 0,
 * stored DEFLATE blocks. No image library or target/device IO is required. */
bool simulator_png(const char *,const uint16_t *,unsigned width,unsigned height);
#endif
