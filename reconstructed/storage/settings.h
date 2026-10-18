#ifndef DMT_SETTINGS_H
#define DMT_SETTINGS_H
#include <stdint.h>
enum settings_field { SETTINGS_THEME=0,SETTINGS_FIELD_BITS_3_4,SETTINGS_LAYOUT,SETTINGS_TEMPERATURE };
uint32_t settings_access(uint16_t *word,enum settings_field field,uint32_t write_flag,int16_t value);
void settings_language_set(uint16_t *,uint16_t);
/* d964 retains the old prefix for unsupported full-width selector values.
 * prefix is the original independent mutable cache, with capacity >=20. */
uint32_t settings_language_access(uint16_t *,char prefix[20],uint32_t write_flag,uint32_t value);
const char *settings_language_prefix(uint16_t);
void settings_auto_off_set(uint16_t *,uint16_t);
uint32_t settings_auto_off_access(uint16_t *word,uint16_t *timeout,uint32_t write_flag,int32_t value); /* da48 */
uint16_t settings_auto_off_seconds(uint16_t);
uint8_t settings_backlight_compare(uint16_t);
#endif
