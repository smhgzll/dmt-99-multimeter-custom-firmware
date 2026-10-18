/* 0xd928,0xd87c,0xdc46: 256-byte settings/history image. Erase/write is only
 * available through an explicitly supplied backend, with original address and
 * status semantics. No calibration contents are synthesized.
 */
#include "persistence.h"
bool settings_image_load(dmt_settings_image *s,const dmt_settings_backend *o)
{return o && o->read && o->read(o->context,DMT_SETTINGS_ADDRESS,s->halfwords,128);}
uint32_t settings_image_commit(const dmt_settings_image *s,const dmt_settings_backend *o,uint32_t status)
{
    if(!o || !o->begin || !o->erase_page || !o->program_halfword || !o->end)return 0;
    if(!o->begin(o->context))return 0;
    if(!o->erase_page(o->context,DMT_SETTINGS_ADDRESS)){o->end(o->context);return 0;}
    for(unsigned i=0;i<128 && status==4;++i)
        status=o->program_halfword(o->context,DMT_SETTINGS_ADDRESS+2*i,s->halfwords[i]);
    o->end(o->context);return status;
}
uint16_t *settings_image_word(dmt_settings_image *s){return s->halfwords+15;}

void settings_image_defaults(dmt_settings_image *s)
{
    /* Initialized SRAM 0x20000120, 17 halfwords copied at 0xdc46. */
    static const uint16_t defaults[]={0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8005,0xdc,0x8064,0x64,0x0,0x64,0x118,0x0};
    for(unsigned i=0;i<128;++i)s->halfwords[i]=i<17?defaults[i]:0;
}
void settings_image_restore_defaults(dmt_settings_image *s,uint8_t *loaded,void *context,void (*logo_remove)(void *))
{
    settings_image_defaults(s);
    logo_remove(context);
    *loaded=1;
}
