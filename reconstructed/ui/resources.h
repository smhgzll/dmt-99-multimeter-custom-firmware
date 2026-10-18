#ifndef DMT_RESOURCES_H
#define DMT_RESOURCES_H
#include "../storage/filesystem.h"
#include "../vendor/tjpgd/tjpgd.h"
#include "graphics.h"
#include "display.h"
/* Workspace belongs to the caller. Avoid the original >11KiB automatic stack
 * demand in resource_draw_jpeg; output RGB565 ordering and return quirks remain. */
typedef struct {
 FIL file;JDEC decoder;
 union {uint32_t align;BYTE bytes[0x1c00];} pool;
 const dmt_canvas *canvas;
 const dmt_display *display; /* optional original rectangle/command/data contract */
 uint16_t x,y;
 FRESULT file_result;JRESULT prepare_result,decode_result;
 uint32_t images_opened,pixels_emitted;
} dmt_resources;
/* Original678c:1 on open failure,0 after any successful open (even bad JPEG).
 * Detailed results expose the original's silent decoder failures to callers. */
int resource_draw_jpeg(dmt_resources *,const char *,uint16_t,uint16_t);
bool resource_view_image(void *,const char *,uint16_t,uint16_t);
#endif
