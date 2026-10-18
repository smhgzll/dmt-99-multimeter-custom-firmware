/* fc9e/fcfc: binary search is compiled with upper index UINT32_MAX (empty
 * conversion table). The first two wrapped reads hit23fe4; the third hits
 * 80023fe4 outside supplied ROM. No generic mapping is substituted. */
#include "codepage.h"
dmt_codepage_result codepage_stock_convert(uint32_t code,uint16_t page,int to_oem)
{
 dmt_codepage_result r={0,DMT_CODEPAGE_DEFINED,0,0};
 if(code<128){r.value=(uint16_t)code;return r;}
 if(page!=936 || (to_oem && code>=65536))return r;
 if(code==0x31a){r.reads=2;return r;} /* key23fe4, value23fe6=0 */
 r.reads=3;r.status=DMT_CODEPAGE_UNMAPPED_ROM;r.fault_address=0x80023fe4;return r;
}
