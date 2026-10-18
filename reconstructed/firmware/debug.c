/* Debug console: 1a682 (USART2 TX-only on PA2, 115200 8N1), 1a6e2 (_write:
 * wait for TC, then send each byte) and the printf at 1aede. The original links
 * its own printf; this is a small bounded formatter covering the conversions
 * the firmware uses (%c %s %d %i %u %x %X %p %f, flags 0 and -, width,
 * precision, l), truncating %f like the original. The console is diagnostics
 * only; nothing reads PA3. */
#include "ch32v30x.h"
#include "firmware.h"

void fw_debug_init(uint32_t baud)
{
    GPIO_InitTypeDef gpio={0};USART_InitTypeDef uart={0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    gpio.GPIO_Pin=GPIO_Pin_2;gpio.GPIO_Speed=GPIO_Speed_50MHz;gpio.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA,&gpio);
    uart.USART_BaudRate=baud;uart.USART_WordLength=USART_WordLength_8b;uart.USART_StopBits=USART_StopBits_1;
    uart.USART_Parity=USART_Parity_No;uart.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    uart.USART_Mode=USART_Mode_Tx;
    USART_Init(USART2,&uart);USART_Cmd(USART2,ENABLE);
}
void fw_debug_write(const char *p,size_t n)
{
    if(!(USART2->CTLR1&USART_CTLR1_UE))return;
    for(size_t i=0;i<n;++i){
        while(!(USART2->STATR&USART_FLAG_TC)){}
        USART2->DATAR=(uint8_t)p[i];
    }
}

typedef struct {char *out;size_t cap,used;} sink;
static void put(sink *s,char c){if(s->used+1<s->cap)s->out[s->used]=c;s->used++;}
static void put_padded(sink *s,const char *body,size_t n,char sign,unsigned width,bool zero,bool left)
{
    size_t total=n+(sign?1:0),pad=width>total?width-total:0;
    if(!left && !zero)while(pad){put(s,' ');pad--;}
    if(sign)put(s,sign);
    if(!left && zero)while(pad){put(s,'0');pad--;}
    for(size_t i=0;i<n;++i)put(s,body[i]);
    while(pad){put(s,' ');pad--;}
}
static size_t utoa_rev(char *buf,unsigned long long v,unsigned base,bool upper)
{
    size_t n=0;
    do{unsigned d=(unsigned)(v%base);buf[n++]=(char)(d<10?'0'+d:(upper?'A':'a')+d-10);v/=base;}while(v);
    return n;
}
int fw_vsnprintf(char *out,size_t cap,const char *fmt,va_list ap)
{
    sink s={out,cap,0};
    while(*fmt){
        char c=*fmt++;
        if(c!='%'){put(&s,c);continue;}
        bool zero=false,left=false;unsigned width=0;int precision=-1;bool longer=false;
        for(;;){if(*fmt=='0')zero=true;else if(*fmt=='-')left=true;else break;fmt++;}
        while(*fmt>='0' && *fmt<='9')width=width*10+(unsigned)(*fmt++-'0');
        if(*fmt=='.'){fmt++;precision=0;while(*fmt>='0' && *fmt<='9')precision=precision*10+(*fmt++-'0');}
        while(*fmt=='l'){longer=true;fmt++;}
        char type=*fmt?*fmt++:0,tmp[40],body[40];size_t n=0;char sign=0;
        switch(type){
        case '%':put(&s,'%');break;
        case 'c':body[0]=(char)va_arg(ap,int);put_padded(&s,body,1,0,width,false,left);break;
        case 's':{
            const char *p=va_arg(ap,const char *);if(!p)p="(null)";
            size_t len=0;while(p[len] && (precision<0 || len<(size_t)precision))len++;
            put_padded(&s,p,len,0,width,false,left);break;
        }
        case 'd':case 'i':{
            long long v=longer?va_arg(ap,long):va_arg(ap,int);
            unsigned long long m=v<0?0ull-(unsigned long long)v:(unsigned long long)v;if(v<0)sign='-';
            n=utoa_rev(tmp,m,10,false);for(size_t i=0;i<n;++i)body[i]=tmp[n-1-i];
            put_padded(&s,body,n,sign,width,zero && !left,left);break;
        }
        case 'u':case 'x':case 'X':case 'p':{
            unsigned long long v=type=='p'?(uintptr_t)va_arg(ap,void *):longer?va_arg(ap,unsigned long):va_arg(ap,unsigned);
            n=utoa_rev(tmp,v,type=='u'?10:16,type=='X');for(size_t i=0;i<n;++i)body[i]=tmp[n-1-i];
            put_padded(&s,body,n,0,width,zero && !left,left);break;
        }
        case 'f':{
            double v=va_arg(ap,double);if(precision<0)precision=6;if(precision>9)precision=9;
            if(v!=v){put_padded(&s,"nan",3,0,width,false,left);break;}
            if(v<0){sign='-';v=-v;}
            if(v>1e18){put_padded(&s,"inf",3,sign,width,false,left);break;}
            unsigned long long scale=1;for(int i=0;i<precision;++i)scale*=10;
            /* The V2.4 printf truncates %f (0.1234567 -> 0.123456, %5.1f 2.25 -> "  2.2"). */
            unsigned long long fixed=(unsigned long long)(v*(double)scale);
            unsigned long long whole=fixed/scale,frac=fixed%scale;
            n=utoa_rev(tmp,whole,10,false);size_t k=0;for(size_t i=0;i<n;++i)body[k++]=tmp[n-1-i];
            if(precision){body[k++]='.';for(int i=precision-1;i>=0;--i){body[k+(size_t)i]=(char)('0'+frac%10);frac/=10;}k+=(size_t)precision;}
            put_padded(&s,body,k,sign,width,zero && !left,left);break;
        }
        default:if(type){put(&s,'%');put(&s,type);}break;
        }
    }
    if(cap)out[s.used<cap?s.used:cap-1]=0;
    return (int)s.used;
}
int fw_printf(const char *fmt,...)
{
    char buffer[160];va_list ap;va_start(ap,fmt);
    int n=fw_vsnprintf(buffer,sizeof buffer,fmt,ap);va_end(ap);
    fw_debug_write(buffer,(size_t)n<sizeof buffer?(size_t)n:sizeof buffer-1);
    return n;
}
