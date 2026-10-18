/* 0x11086 refactored from six blocking digit editors to one resumable editor.
 * The caller suspends the normal menu while active. Event 6 commits the edit
 * and remains pending so the outer menu can close. Sign/magnitude negative zero
 * on commit and clamp-before-peer ordering match the original.
 */
#include "thresholds.h"
int32_t threshold_decode(uint16_t v){return (v&0x8000)?-(int32_t)(v&0x7fff):(v&0x7fff);}
uint16_t threshold_encode(int32_t v){return v<1?(uint16_t)(-v)|0x8000:(uint16_t)v;}
bool threshold_editor_begin(dmt_threshold_editor *e,const uint16_t w[6],uint8_t id)
{
    if(id<6 || id>11)return false;
    e->index=id-6;e->column=1;e->active=true;
    e->value=threshold_decode(w[e->index]);e->peer=threshold_decode(w[e->index^1]);return true;
}
bool threshold_editor_step(dmt_threshold_editor *e,uint16_t w[6],uint8_t *event)
{
    if(!e->active)return true;
    if(*event==5 || *event==6){
        if(*event==5)*event=0;
        w[e->index]=threshold_encode(e->value);e->active=false;return true;
    }
    if(*event==3){e->column=e->column==4?1:e->column+1;*event=0;}
    else if(*event==4){e->column=e->column==1?4:e->column-1;*event=0;}
    if(*event==1 || *event==2){
        int32_t step=e->column==2?100:e->column==3?10:1;
        e->value=e->column==1?-e->value:e->value+(*event==1?-step:step);*event=0;
    }
    int minimum=e->index>=4?-55:-999;
    if(e->value<minimum)e->value=minimum;
    if(e->value>999)e->value=999;
    if((e->index&1)==0){if(e->value>=e->peer)e->value=e->peer-1;}
    else if(e->value<=e->peer)e->value=e->peer+1;
    return false;
}
static void draw_row(const dmt_threshold_editor *e,const dmt_canvas *c,uint8_t layout,bool selected,bool negative)
{
    static const uint8_t y[2][6]={{43,64,108,130,172,195},{30,57,106,134,184,213}};
    uint16_t row=y[layout!=0][e->index];unsigned value=e->value<0?(unsigned)-e->value:(unsigned)e->value;
    uint8_t ch[4]={(uint8_t)(negative?'-':'+'),(uint8_t)('0'+(value/100)%10),(uint8_t)('0'+(value/10)%10),(uint8_t)('0'+value%10)};
    uint16_t x[4]={212,227,e->index==2 || e->index==3?248:242,e->index==2 || e->index==3?263:257};
    for(unsigned i=0;i<4;++i){
        if(selected && e->column==i+1)graphics_ascii(c,x[i],row,ch[i],16,0xffff,0x1f,false);
        else graphics_measurement_char(c,x[i],row,ch[i],16,selected?0xfd40:0xffff,false);
    }
    if(e->index==2 || e->index==3)graphics_measurement_char(c,239,row,'.',16,selected?0xfd40:0xffff,false);
}
void threshold_editor_draw(const dmt_threshold_editor *e,const dmt_canvas *c,uint8_t layout,bool selected)
{draw_row(e,c,layout,selected,e->value<1);}
void thresholds_draw(const uint16_t w[6],const dmt_canvas *c,uint8_t layout)
{
    for(unsigned i=0;i<6;++i){dmt_threshold_editor e={.value=threshold_decode(w[i]),.index=i};
        /* The static display uses the encoded sign, including negative zero. */
        draw_row(&e,c,layout,false,(w[i]&0x8000)!=0);
    }
}
bool threshold_alarm(const uint16_t w[6],uint8_t mode,float value,bool fahrenheit,uint8_t *alarm)
{
    unsigned group;
    if(mode==0x0c || mode==0x0d)group=0;
    else if(mode==0x10 || mode==0x15){group=2;value*=100;}
    else if(mode==0x12)group=4;
    else return false;
    int32_t low=threshold_decode(w[group]),high=threshold_decode(w[group+1]);
    if(mode==0x12 && fahrenheit){low=(int32_t)((double)low*1.8+32);high=(int32_t)((double)high*1.8+32);value=(float)((double)value*1.8+32);}
    uint8_t next=*alarm;
    if((float)low<value && (float)high>value)next=0;
    else if((float)low>value)next=1;
    else if((float)high<value)next=2;
    if(next==*alarm)return false;
    *alarm=next;return true;
}
