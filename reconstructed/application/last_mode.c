#include "last_mode.h"
bool last_mode_valid(uint8_t view,uint8_t mode)
{
 if(view==1)return mode>=9 && mode<=0x16;
 if(view==2)return (mode>=0xa && mode<=0x12) || mode==0x15 || mode==0x16;
 if(view==3)return mode==0xb || mode==0xc || mode==0xd || mode==0x10 || mode==0x15 || mode==0x12;
 return false;
}
#if DMT_LAST_MODE
static uint32_t crc(const uint8_t *b,unsigned n)
{uint32_t c=~0u;while(n--){c^=*b++;for(unsigned j=0;j<8;j++)c=c&1?(c>>1)^0xedb88320u:c>>1;}return ~c;}
bool last_mode_load(const dmt_last_mode_backend *p,uint8_t view,uint8_t *mode)
{
 uint8_t b[16];if(!p || !p->read || !p->read(p->context,b))return false;
 if(b[0]!='D' || b[1]!='M' || b[2]!='9' || b[3]!='9' || b[4]!=1 || b[6]!=view)return false;
 uint32_t expected=(uint32_t)b[12]|(uint32_t)b[13]<<8|(uint32_t)b[14]<<16|(uint32_t)b[15]<<24;
 if(expected!=crc(b,12) || !last_mode_valid(view,b[5]))return false;
 *mode=b[5];return true;
}
bool last_mode_save(const dmt_last_mode_backend *p,uint8_t view,uint8_t mode)
{
 if(!last_mode_valid(view,mode) || !p || !p->write)return false;
 uint8_t old;if(last_mode_load(p,view,&old) && old==mode)return true;
 uint8_t b[16]={'D','M','9','9',1,mode,view,0,0,0,0,0};uint32_t c=crc(b,12);
 for(unsigned i=0;i<4;i++)b[12+i]=(uint8_t)(c>>(i*8));
 return p->write(p->context,b);
}
bool last_mode_forget(const dmt_last_mode_backend *p)
{const uint8_t empty[16]={0};return p && p->write && p->write(p->context,empty);}
bool last_mode_commit(const dmt_last_mode_backend *p)
{return p && (!p->commit || p->commit(p->context));}
bool last_mode_restore(dmt_control *s,const dmt_last_mode_backend *p,void (*send)(void *,const uint8_t[10]),void *context)
{
 uint8_t mode;if(!send || !last_mode_load(p,s->view,&mode))return false;
 for(unsigned i=0;i<4;i++)s->regular_selector[i]=0;
 for(unsigned i=0;i<3;i++)s->record_selector[i]=0;
 const uint8_t groups[3][5]={{0xc,0xd,0xb,0xa,0xe},{0x11,0x10,0x16,0x15,0},{0x12,0xf,9,0x13,0}};
 for(unsigned g=0;g<3;g++)for(unsigned i=0;i<5;i++)if(groups[g][i]==mode){s->regular_selector[g]=i+1;if(g<3 && i<(g==2?2u:5u))s->record_selector[g]=i+1;}
 if(mode==0x14)s->regular_selector[3]=1;
 s->monitor_selector=mode==0xc;
 s->event=0; /* suppress stock default command; mode changes only after FD ACK */
 uint8_t packet[10]={0xaa,0x55,5,mode,0,0,0,0,0,(uint8_t)(mode+5)};send(context,packet);return true;
}
#else
bool last_mode_forget(const dmt_last_mode_backend *p){(void)p;return false;}
bool last_mode_load(const dmt_last_mode_backend *p,uint8_t view,uint8_t *mode){(void)p;(void)view;(void)mode;return false;}
bool last_mode_save(const dmt_last_mode_backend *p,uint8_t view,uint8_t mode){(void)p;(void)view;(void)mode;return false;}
bool last_mode_restore(dmt_control *s,const dmt_last_mode_backend *p,void (*send)(void *,const uint8_t[10]),void *c){(void)s;(void)p;(void)send;(void)c;return false;}
bool last_mode_commit(const dmt_last_mode_backend *p){(void)p;return false;}
#endif
