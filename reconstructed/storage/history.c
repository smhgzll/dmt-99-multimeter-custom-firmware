/* 0xda9c/0xdbac: count at 0x20000040, thirty four-byte records at 0x20000042.
 * Bounds checking is an explicit reconstruction guard around the original float-to-int conversion.
 */
#include "history.h"
bool history_encode(uint8_t mode,uint8_t unit,float value,uint8_t overload,dmt_history_record *out)
{
    if(!out || mode>63 || unit>63 || overload>2)return false;
    uint16_t meta=(uint16_t)(mode|((uint16_t)unit<<6));
    if(overload)value=overload==1?65535.0f:61166.0f;
    else{
        if(value<0){meta|=0x1000;value=-value;}
        if(value<10){value*=1000;meta|=0x6000;}
        else if(value<100){value*=100;meta|=0x4000;}
        else if(value<1000){value*=10;meta|=0x2000;}
    }
    if(!(value>=0 && value<=65535))return false;
    out->metadata=meta;out->magnitude=(uint16_t)value;return true;
}
float history_decode(dmt_history_record r)
{
    float v=r.magnitude;
    switch((r.metadata>>13)&3){case 3:v/=1000;break;case 2:v/=100;break;case 1:v/=10;break;}
    return (r.metadata&0x1000)?-v:v;
}
void history_push(dmt_history *h,dmt_history_record r)
{
    for(unsigned i=DMT_HISTORY_CAPACITY-1;i>0;--i)h->records[i]=h->records[i-1];
    h->records[0]=r;if(h->count<DMT_HISTORY_CAPACITY)++h->count;
}
