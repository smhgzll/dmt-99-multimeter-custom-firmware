/* Last measurement mode record (application/last_mode.c, release builds) in
 * the unused tail of the V2.4 settings page: halfwords 120..127 of the
 * settings image (0x080400f0..0x080400ff). V2.4 uses halfwords 0..76 (settings
 * and 30 history records); it loads and commits all 128 halfwords, so the
 * record survives a rollback to V2.4 and back. A factory reset clears it with
 * the rest of the page.
 *
 * read()/write() work on the RAM settings image; the record reaches Flash with
 * the settings commit at power-off (no extra erase cycle, no other page).
 */
#include "firmware.h"
#include "runtime.h"

#if DMT_LAST_MODE
enum {FIRST=120};
static bool read_record(void *context,uint8_t out[16])
{
    const dmt_settings_image *s=context;
    for(unsigned i=0;i<8;++i){out[2*i]=(uint8_t)s->halfwords[FIRST+i];out[2*i+1]=(uint8_t)(s->halfwords[FIRST+i]>>8);}
    return true;
}
static bool write_record(void *context,const uint8_t record[16])
{
    dmt_settings_image *s=context;
    for(unsigned i=0;i<8;++i)s->halfwords[FIRST+i]=(uint16_t)(record[2*i]|record[2*i+1]<<8);
    return true;
}
const dmt_last_mode_backend fw_last_mode_backend={&fw_app.settings,read_record,write_record,0};
#endif
