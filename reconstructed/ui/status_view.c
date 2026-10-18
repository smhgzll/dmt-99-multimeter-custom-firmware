/* 0x14e48,0x14d62,0xc11a,0x1014e,0xfe08: exact resource names/coordinates.
 * Resources still require the device's external filesystem; missing images are
 * reported by its backend. No replacement artwork is generated.
 */
#include "status_view.h"
#include "config.h"
#if DMT_THEMES
#include "theme_ui.h"
#endif
static void image(const dmt_status_view *v,const char *p,uint16_t x,uint16_t y)
{if(v->image)v->image(v->context,p,x,y);}
void status_battery(const dmt_status_view *v,int p,uint8_t layout)
{
    if(layout){
        static const char *const cells[]={"cell/cell0.jpg","cell/cell1.jpg","cell/cell2.jpg","cell/cell3.jpg","cell/cell4.jpg","cell/cell5.jpg","cell/cell6.jpg","cell/cell7.jpg","cell/cell8.jpg","cell/cell9.jpg"};
        unsigned n=p<11?0:p<21?1:p<31?2:p<41?3:p<51?4:p<61?5:p<71?6:p<81?7:p<91?8:9;
        image(v,cells[n],60,1);return;
    }
    unsigned bars=p<13?0:p<26?1:p<38?2:p<51?3:p<63?4:p<76?5:p<88?6:7;
    if(v->canvas)for(unsigned i=1;i<=7;++i)graphics_fill(v->canvas,140+6*(i-1),6,144+6*(i-1),16,i<=bars?0x7e0:(i==1 && bars==0)?0xf800:0x4228);
}
void status_unit(const dmt_status_view *v,uint16_t x,uint16_t y,uint8_t unit,uint8_t *last)
{
    static const char *const units[]={"units/v.jpg","units/mv.jpg","units/ua.jpg","units/ma.jpg","units/a.jpg","units/r.jpg","units/kr.jpg","units/mr.jpg",0,"units/nf.jpg","units/uf.jpg","units/mf.jpg","units/hz.jpg","units/khz.jpg","units/mhz.jpg","units/c.jpg"};
    if(*last==unit)return;
    *last=unit;if(v->canvas)graphics_fill(v->canvas,x,y,x+60,y+40,0);
    if(unit>=6 && unit<=21 && units[unit-6])image(v,units[unit-6],x,y);
}
void status_live(const dmt_status_view *v,bool active,uint8_t *last)
{
    uint8_t id=active?24:23;if(*last==id)return;
    *last=id;image(v,active?"live1.jpg":"live.jpg",0,0);
}
void status_ncv(const dmt_status_view *v,uint8_t level,uint8_t layout)
{
    static const char *const base[]={"0:/ncv1.jpg","0:/ncv2.jpg","0:/ncv3.jpg"};
    static const char *const wave[3][3][2]={
      {{"0:/motif2/ncv1_up_1.jpg","0:/motif2/ncv1_do_1.jpg"},{"0:/motif2/ncv1_up_2.jpg","0:/motif2/ncv1_do_2.jpg"},{"0:/motif2/ncv1_up_3.jpg","0:/motif2/ncv1_do_3.jpg"}},
      {{"0:/motif2/ncv2_up_1.jpg","0:/motif2/ncv2_do_1.jpg"},{"0:/motif2/ncv2_up_2.jpg","0:/motif2/ncv2_do_2.jpg"},{"0:/motif2/ncv2_up_3.jpg","0:/motif2/ncv2_do_3.jpg"}},
      {{"0:/motif2/ncv3_up_1.jpg","0:/motif2/ncv3_do_1.jpg"},{"0:/motif2/ncv3_up_2.jpg","0:/motif2/ncv3_do_2.jpg"},{"0:/motif2/ncv3_up_3.jpg","0:/motif2/ncv3_do_3.jpg"}}};
    static const char *const waves_a[3][3]={{"0:/ncv1_1.jpg","0:/ncv1_2.jpg","0:/ncv1_3.jpg"},{"0:/ncv2_1.jpg","0:/ncv2_2.jpg","0:/ncv2_3.jpg"},{"0:/ncv3_1.jpg","0:/ncv3_2.jpg","0:/ncv3_3.jpg"}};
    static const uint8_t x[]={95,65,30},top[]={90,60,20},bottom[]={140,160,190};
    if(!level){image(v,layout?"0:/ncv0.jpg":"0:/motif2/ncv0.jpg",0,0);return;}
    if(level>4)return;
    unsigned index=level>=3?2:level-1;image(v,base[index],0,0);
    for(unsigned i=0;i<3;++i){
        image(v,layout?waves_a[index][i]:wave[index][i][0],x[i],top[i]);
        image(v,layout?waves_a[index][i]:wave[index][i][1],x[i],bottom[i]);
        if(i<2 && v->delay_ms)v->delay_ms(v->context,200);
    }
}

#include "format.h"
void status_charging(const dmt_status_view *v,const char *prefix)
{
    char path[64];if(v->canvas)graphics_fill(v->canvas,0,0,319,239,0);
    image(v,"menu/recharge.jpg",123,22);
    if(prefix && dmt_format(path,sizeof path,"%s%s",prefix,"recharge1.jpg"))image(v,path,0,87);
}
void status_low_battery_countdown(const dmt_status_view *v,const char *prefix,uint8_t language)
{
    char path[64];if(prefix && dmt_format(path,sizeof path,"%s%s",prefix,"close.jpg"))image(v,path,41,64);
    for(int i=3;i>0;--i){dmt_format(path,sizeof path,"time/%d.jpg",i);image(v,path,134,language==0 || language==3?95:124);if(v->delay_ms)v->delay_ms(v->context,1000);}
}
void status_language_choice(const dmt_status_view *v,uint8_t choice,bool initial,bool confirmed)
{
    if(initial){
        if(v->canvas)graphics_fill(v->canvas,0,0,319,239,0);
        image(v,"menu/chinese.jpg",42,112);image(v,"menu/english.jpg",186,112);
    }
    if(v->canvas){graphics_rectangle(v->canvas,37,103,169,138,choice==1?0xfd40:0);graphics_rectangle(v->canvas,181,103,313,138,choice==2?0xfd40:0);}
    if(confirmed)image(v,choice==2?"menu/english_.jpg":"menu/chinese_.jpg",choice==2?186:42,112);
}
void status_scrollbar(const dmt_status_view *v,uint8_t p,uint8_t layout)
{
    if(!v->canvas)return;
    static const uint8_t y0[]={12,43,74,105,136,167},y1[]={72,103,134,165,196,227};
    static const uint8_t a0[]={6,34,62,90,118,149},a1[]={87,115,143,171,199,230};
    if(!layout){graphics_fill(v->canvas,317,11,319,228,0x4228);if(p>=1 && p<=6)graphics_line(v->canvas,318,y0[p-1],318,y1[p-1],0x1c9f);}
    else{graphics_fill(v->canvas,315,5,317,234,0x4228);if(p>=1 && p<=6)graphics_line(v->canvas,316,a0[p-1],316,a1[p-1],0xfd40);}
}
#if DMT_ABOUT_FW_VERSION
/* Release builds show the product name and their own version instead of
 * V2.4's label so an installed update can be recognised on the About page. The
 * text is centred on the original label (x 180..263) using graphics_text_box's
 * 16 px advances. */
void status_about_version(const dmt_status_view *v)
{
    static const char text[]=DMT_PRODUCT_NAME " V" DMT_FW_VERSION;
    uint16_t width=0;
    for(const char *p=text;*p;++p)width=(uint16_t)(width+(*p=='.'?4:8)+(((*p>='X' && *p<='Z') || *p=='J')?6:0));
    if(v->canvas)graphics_text_box(v->canvas,(uint16_t)(222-width/2),163,(uint16_t)(width+8),20,16,0xffff,text,true);
#if DMT_THEMES
    if(v->canvas)theme_about_hint(v->canvas);
#endif
}
#else
void status_about_version(const dmt_status_view *v)
{if(v->canvas)graphics_text_box(v->canvas,180,163,80,20,16,0xffff,"DMT-99 V2.4",true);}
#endif
