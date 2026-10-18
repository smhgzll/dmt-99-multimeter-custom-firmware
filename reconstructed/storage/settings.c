/* 0xd944,0xd9e6,0xda04,0xda28. Packed word at 0x2000003e.
 * The temperature setter clears bit 6 as well as bit 5 in the original;
 * retain that observable behavior until its interaction with auto-off is understood.
 */
#include "settings.h"
uint32_t settings_access(uint16_t *word,enum settings_field field,uint32_t write_flag,int16_t value)
{
    static const uint8_t shifts[]={8,3,10,5};
    static const uint16_t clear_masks[]={0xfcff,0xffe7,0xf3ff,0xff9f};
    if((unsigned)field>3)return 0;
    unsigned shift=shifts[field];
    if(!write_flag)return (*word>>shift)&(field==SETTINGS_TEMPERATURE?1u:3u);
    *word=(uint16_t)((*word&clear_masks[field])|((uint32_t)(int32_t)value<<shift));
    return field==SETTINGS_LAYOUT?0xfffff3ffu:write_flag;
}

/* 0xd964 and 0xda48. These setters deliberately do not mask input values:
 * the original field writers also allow adjacent-bit spill for invalid values.
 */
void settings_language_set(uint16_t *word,uint16_t value)
{*word=(uint16_t)((*word&0xfff8u)|value);}
uint32_t settings_language_access(uint16_t *word,char prefix[20],uint32_t write_flag,uint32_t value)
{
    if(!write_flag)return *word&7;
    settings_language_set(word,(uint16_t)value);
    /* The original switch examines all32 bits, not just the stored low bits. */
    if(value>5)return write_flag;
    const char *path=settings_language_prefix((uint16_t)value);uint32_t n=0;
    do{prefix[n]=path[n];}while(path[n++]);
    return n; /* 969e's successful count includes the terminating NUL. */
}
const char *settings_language_prefix(uint16_t word)
{
    static const char *const paths[]={"menu/chinese/","menu/english/","menu/german/",
        "menu/japanese/","menu/portuguese/","menu/russian/"};
    unsigned language=word&7;return language<6?paths[language]:0;
}
void settings_auto_off_set(uint16_t *word,uint16_t value)
{*word=(uint16_t)((*word&0xff3fu)|(value<<6));}
uint32_t settings_auto_off_access(uint16_t *word,uint16_t *timeout,uint32_t write_flag,int32_t value)
{
    if(!write_flag)return (*word>>6)&3;
    *word=(uint16_t)((*word&0xff3fu)|((uint32_t)value<<6));
    /* Invalid values spill into adjacent bits but leave the old timeout. */
    if((uint32_t)value<4)*timeout=settings_auto_off_seconds(*word);
    return write_flag;
}
uint16_t settings_auto_off_seconds(uint16_t word)
{static const uint16_t seconds[]={0,300,900,1800};return seconds[(word>>6)&3];}
uint8_t settings_backlight_compare(uint16_t word)
{unsigned value=(word>>3)&3;return value==1?50:value==2?70:0;}
