/* Original BE78/9774/96DE LCD-to-text/value behavior. Kept in the portable
 * driver so firmware without any UI/controller can use the same tested parser.
 * FD frames are intentionally parsed too: the stock foreground does that.
 */
#include "measurement_asic.h"
float measurement_parse_number(const char *text)
{
    char filtered[16];unsigned n=0;
    for(;*text && n<sizeof(filtered)-1;++text)
        if((*text>='0' && *text<='9') || *text=='-' || *text=='.')filtered[n++]=*text;
    filtered[n]=0;
    const char *p=filtered;int sign=1;
    if(*p=='-'){sign=-1;++p;}else if(*p=='+')++p;
    uint32_t whole=0,fraction=0,scale=1;bool decimal=false;
    for(;*p;++p){
        if(*p=='.')decimal=true;
        else if(decimal){scale*=10;fraction=fraction*10+(int)*p-'0';}
        else whole=whole*10+(int)*p-'0';
    }
    float f=(float)(int32_t)fraction/(float)(int32_t)scale;
    f=f+(float)(int32_t)whole;
    return (float)sign*f;
}
bool measurement_parse_reading(measurement_reading *s,const uint8_t b[13])
{
    measurement_frame f;
    /* 0xbe78 does not reject 0xfd event frames; its caller also parses events. */
    if(b[0]!=0x5a || b[1]!=0xa5)return false;
    uint8_t copy[13];for(unsigned i=0;i<13;++i)copy[i]=b[i];
    if(copy[2]==0xfd)copy[2]=0xfc;
    measurement_decode_frame(copy,13,&f);
    s->ac_dc_code=(b[2]&2)?2:(b[2]&1)?1:255;s->status_high=b[7]>>7;
    char digits[4];bool changed=false;uint8_t numeric=s->numeric;
    for(unsigned i=0;i<4;++i){
        uint8_t code=measurement_segment_decode((uint8_t)((b[2+i]&0xf0)|(b[3+i]&15)));
        if((int8_t)code<10){digits[i]=(char)((int8_t)code+'0');changed=true;numeric=1;}
        else if(code>=10 && code<=16){static const char a[]="AUTOLE-";digits[i]=a[code-10];changed=true;numeric=0;}
        else if(code==17)digits[i]=(s->mode==0x12 || s->mode==0x11 || s->mode==0x16)?'0':' ';
        else if(code<=21){static const char a[]="CPSr";digits[i]=a[code-18];changed=true;numeric=0;}
        else digits[i]='X';
    }
    if(changed)s->numeric=numeric;
    unsigned j=0;s->text[j++]=(b[2]&0x10)?'-':' ';
    unsigned decimal=(b[3]&0x10)?1:(b[4]&0x10)?2:(b[5]&0x10)?3:0;
    for(unsigned i=0;i<4;++i){s->text[j++]=digits[i];if(decimal==i+1)s->text[j++]='.';}
    if(!decimal)s->text[j++]=' ';
    /* The original prints a seventh %c whose value is0, then the formatter
     * writes its own terminator. Both bytes are visible in the16-byte snapshot. */
    s->text[j]=0;s->text[j+1]=0;s->value=measurement_parse_number(s->text);s->unit=f.unit_code;
    return true;
}
