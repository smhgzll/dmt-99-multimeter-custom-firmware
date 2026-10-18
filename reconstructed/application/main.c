/* Offline application replay and link harness. No MMIO is initialized and no
 * settings backend is bound. The SDK port is available in libreconstructed.a;
 * missing original UI/resource routines remain counted address boundaries.
 * This ELF is an analysis artifact, not a flashable firmware release.
 */
#include "application.h"
static dmt_application application;
volatile uint32_t reconstruction_selfcheck;
static void pixel(void *ctx,uint16_t x,uint16_t y,uint16_t color)
{(void)ctx;reconstruction_selfcheck=(reconstruction_selfcheck*33)^((uint32_t)x<<16)^y^color;}
static void send(void *ctx,const uint8_t p[10])
{(void)ctx;for(unsigned i=0;i<10;++i)reconstruction_selfcheck=reconstruction_selfcheck*33+p[i];}
static bool unresolved(void *ctx,uint32_t pc,const dmt_application *a)
{(void)ctx;(void)a;reconstruction_selfcheck^=pc;return false;}
int main(void)
{
    static const dmt_canvas canvas={0,pixel,320,240,0};
    static const dmt_application_io io={.send=send,.unresolved=unresolved,.canvas=&canvas};
    dmt_settings_image image;settings_image_defaults(&image);image.halfwords[0]=0;
    settings_access(settings_image_word(&image),SETTINGS_THEME,1,1);
    application_init(&application,&image,&io);application_step(&application,&io);
    application.control.event=6;application_step(&application,&io);
    application.menu.state=6;application.control.event=5;application_step(&application,&io);
    application.control.event=3;application_step(&application,&io);
    application.control.event=2;application_step(&application,&io);
    application.control.event=6;application_step(&application,&io);application_step(&application,&io);
    for(;;)__asm__ volatile("nop");
}
