#include "format.h"
#include <stdarg.h>
#include <stdbool.h>
static unsigned digits(uint32_t v,unsigned base)
{unsigned n=1;while(v>=base){v/=base;++n;}return n;}
static unsigned number(char *out,uint32_t value,unsigned base,unsigned count,bool upper)
{
    unsigned pos=count;while(pos){unsigned d=value%base;value/=base;out[--pos]=(char)(d<10?'0'+d:(upper?'A':'a')+d-10);}return count;
}
size_t dmt_format(char *out,size_t cap,const char *fmt,...)
{
    if(!cap)return 0;
    va_list ap;va_start(ap,fmt);size_t used=0;bool failed=false;
    while(*fmt && !failed){
        char text[192];unsigned n=0,width=0,precision=6,sign_size=0;bool zero=false,left=false;
        char type=*fmt++;
        if(type!='%')text[n++]=type;
        else{
            type=*fmt++;
            if(type=='%')text[n++]='%';
            else{
                if(type=='0'){zero=true;type=*fmt++;}else if(type=='-'){left=true;type=*fmt++;}
                while(type>='0' && type<='9'){width=(width*10+type-'0')&255;type=*fmt++;}
                if(type=='.'){precision=0;type=*fmt++;while(type>='0' && type<='9'){precision=(precision*10+type-'0')&255;type=*fmt++;}}
                if(type=='l')type=*fmt++;
                if(type=='c')text[n++]=(char)va_arg(ap,int);
                else if(type=='s'){
                    const char *p=va_arg(ap,const char *);if(!p){failed=true;break;}
                    while(*p && n<sizeof text)text[n++]=*p++;
                    if(*p){failed=true;break;}
                }else if(type=='d' || type=='u' || type=='x' || type=='X'){
                    uint32_t v=type=='d'?(uint32_t)va_arg(ap,int):va_arg(ap,unsigned);unsigned base=(type=='x' || type=='X')?16:10;
                    if(type=='d' && (int32_t)v<0){v=0-v;sign_size=1;}
                    n=number(text,v,base,digits(v,base),type=='X');

                }else if(type=='f'){
                    double value=va_arg(ap,double);
                    if(value<0){value=-value;sign_size=1;}
                    if(!(value>=0 && value<4294967295.0) || precision>9){failed=true;break;}
                    uint32_t whole=(uint32_t)value;
                    n=number(text,whole,10,digits(whole,10),false);
                    if(precision){
                        text[n++]='.';uint32_t scale=1;for(unsigned i=0;i<precision;++i)scale*=10;
                        uint32_t fraction=(uint32_t)((value-(double)whole)*(double)scale+0.5);
                        /* Original truncates carry: 9.9999 -> 9.000 at 3 places. */
                        n+=number(text+n,fraction,10,precision,false);
                    }
                }else if(type)text[n++]=type;
                else break;
            }
        }
        unsigned pad=width>n+sign_size?width-n-sign_size:0;
        if(used+n+pad+sign_size>=cap){failed=true;break;}
        if(!left)for(unsigned i=0;i<pad;++i)out[used++]=zero?'0':' ';
        if(sign_size)out[used++]='-';
        for(unsigned i=0;i<n;++i)out[used++]=text[i];
        if(left)for(unsigned i=0;i<pad;++i)out[used++]=' ';
    }
    va_end(ap);out[used]=0;return failed?0:used+1;
}
void measurement_format_value(char *out,size_t cap,float value,uint8_t mode,uint8_t unit)
{
    /* 0x99cc: four significant digits; temperature/uA display adds zero columns. */
    bool padded=mode==0x12 || ((mode==0x11 || mode==0x16) && unit==8);
    bool negative=value<0;float magnitude=negative?-value:value;
    unsigned decimals=magnitude>=1000?0:magnitude>=100?1:magnitude>=10?2:3;
    if(padded)decimals=0;
    char fmt[24];unsigned i=0;
    if(padded && magnitude<1000){
        fmt[i++]='%';fmt[i++]='c';
        unsigned zeros=magnitude>=100?1:magnitude>=10?2:3;
        for(unsigned j=0;j<zeros;++j)fmt[i++]='0';
    }else if(!negative){fmt[i++]='%';fmt[i++]='c';}
    fmt[i++]='%';fmt[i++]='0';fmt[i++]='.';fmt[i++]=(char)('0'+decimals);fmt[i++]='f';fmt[i]=0;
    if(padded && magnitude<1000)dmt_format(out,cap,fmt,negative?'-':' ',(double)magnitude);
    else if(!negative)dmt_format(out,cap,fmt,' ',(double)value);
    else dmt_format(out,cap,fmt,(double)value);
}
