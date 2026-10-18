#include "dmt99_asic.h"
#include <string.h>

uint8_t dmt99_decode_segment(uint8_t value)
{
    static const uint8_t patterns[] = {
        0xeb,0x0a,0xad,0x8f,0x4e,0xc7,0xe7,0x8a,0xef,0xcf,
        0xee,0x23,0x65,0x27,0x61,0xe5,0x04,0x00,0xe1,0xec
    };
    value &= 0xef;
    for (uint8_t i=0; i<sizeof patterns; ++i)
        if (value==patterns[i]) return i;
    return value==0x24 ? 21 : 255;
}

static uint8_t unit_code(const uint8_t *b)
{
    if (b[8]&2) return (b[7]&1) ? 7 : 6;
    if (b[7]&0x20) return (b[6]&0x40) ? 12 : (b[7]&4) ? 13 : 11;
    if (b[8]&1) return (b[6]&0x10) ? 8 : (b[7]&1) ? 9 : 10;
    if (b[7]&0x10) return (b[6]&0x20) ? 15 : (b[6]&0x10) ? 16 : 17;
    if (b[8]&0x20) return 21;
    if (b[8]&4) return (b[6]&0x40) ? 19 : (b[7]&4) ? 20 : 18;
    return 1;
}

const char *dmt99_unit_resource_name(uint8_t code)
{
    switch (code) {
    case 6:return "V";case 7:return "mV";case 8:return "uA";
    case 9:return "mA";case 10:return "A";case 11:return "ohm";
    case 12:return "kohm";case 13:return "Mohm";case 15:return "nF";
    case 16:return "uF";case 17:return "mF";case 18:return "Hz";
    case 19:return "kHz";case 20:return "MHz";
    case 21:return "temperature_C_or_F";default:return "unknown";
    }
}

bool dmt99_parse_rx(const uint8_t *data, size_t n, dmt99_rx *out)
{
    static const char alphabet[]="0123456789AUTOLE- CPSr";
    if (!out) return false;
    memset(out,0,sizeof *out);
    if (!data || n>DMT99_MAX_RX_BYTES) return false;
    memcpy(out->raw,data,n);out->length=(uint8_t)n;
    if (n==10 && data[0]==0xaa && data[1]==0x55 && data[2]==0xfd) {
        out->kind=DMT99_RX_EVENT;out->event_id=data[3];return true;
    }
    if ((n!=12 && n!=13) || data[0]!=0x5a || data[1]!=0xa5) return false;
    if (data[2]==0xfd) {
        out->kind=DMT99_RX_EVENT;out->event_id=data[3];return true;
    }
    out->kind=DMT99_RX_DISPLAY;
    out->negative=(data[2]&0x10)!=0;
    out->decimal_after=(data[3]&0x10)?1:(data[4]&0x10)?2:(data[5]&0x10)?3:0;
    out->unit_code=unit_code(data);
    out->ac_dc_code=(data[2]&2)?2:(data[2]&1)?1:255;
    out->status_b7_bit7=(data[7]&0x80)!=0;
    out->numeric=true;out->divisor=1;
    bool have_digit=false;
    unsigned first_digit=4;
    unsigned pos=0;
    if (out->negative) out->text[pos++]='-';
    for (unsigned i=0; i<4; ++i) {
        uint8_t symbol=dmt99_decode_segment((uint8_t)((data[2+i]&0xf0)|(data[3+i]&15)));
        out->symbols[i]=symbol;
        out->text[pos++]=(symbol<sizeof alphabet-1)?alphabet[symbol]:'?';
        if (symbol<10) {
            if (!have_digit) first_digit=i;
            have_digit=true;out->mantissa=out->mantissa*10+symbol;
        } else if (!(symbol==17 && !have_digit)) out->numeric=false;
        if (out->decimal_after==i+1) out->text[pos++]='.';
    }
    out->text[pos]='\0';
    if (!have_digit || (out->decimal_after && first_digit>=out->decimal_after)) out->numeric=false;
    if (out->decimal_after)
        for (unsigned i=out->decimal_after; i<4; ++i) out->divisor*=10;
    if (out->negative) out->mantissa=-out->mantissa;
    if (!out->numeric) {out->mantissa=0;out->divisor=0;}
    return true;
}

bool dmt99_build_firmware_template(uint8_t id, uint8_t out[DMT99_TX_BYTES])
{
    if (!out || !(id==0 || id==1 || id==3 || id==8 || (id>=9 && id<=0x16))) return false;
    memset(out,0,DMT99_TX_BYTES);
    out[0]=0xaa;out[1]=0x55;out[2]=5;out[3]=id;
    out[9]=id ? (uint8_t)(id+5) : 0x11;
    return true;
}

bool dmt99_is_resistance_open_pattern(const dmt99_rx *r)
{
    static const uint8_t pattern[]={17,0,14,17};
    return r && r->kind==DMT99_RX_DISPLAY && r->unit_code==13 &&
        r->decimal_after==2 && memcmp(r->symbols,pattern,sizeof pattern)==0;
}

bool dmt99_host_ui_status(const dmt99_rx *r, uint8_t mode, uint8_t *out)
{
    if (!r || !out || r->kind!=DMT99_RX_DISPLAY) return false;
    uint8_t value=0;
    if (mode==0x09) {
        for (unsigned i=0;i<4;++i) value+=(r->symbols[i]==16);
    } else if (mode==0x13) value=(r->symbols[0]!=14);
    else return false;
    *out=value;
    return true;
}

bool dmt99_value_si(const dmt99_rx *r, double *out, dmt99_quantity *quantity)
{
    if (!r || !out || !quantity || r->kind!=DMT99_RX_DISPLAY || !r->numeric || !r->divisor) return false;
    double scale;
    dmt99_quantity kind;
    switch (r->unit_code) {
    case 6:kind=DMT99_QUANTITY_VOLT;scale=1.0;break;
    case 7:kind=DMT99_QUANTITY_VOLT;scale=1e-3;break;
    case 8:kind=DMT99_QUANTITY_AMPERE;scale=1e-6;break;
    case 9:kind=DMT99_QUANTITY_AMPERE;scale=1e-3;break;
    case 10:kind=DMT99_QUANTITY_AMPERE;scale=1.0;break;
    case 11:kind=DMT99_QUANTITY_OHM;scale=1.0;break;
    case 12:kind=DMT99_QUANTITY_OHM;scale=1e3;break;
    case 13:kind=DMT99_QUANTITY_OHM;scale=1e6;break;
    case 15:kind=DMT99_QUANTITY_FARAD;scale=1e-9;break;
    case 16:kind=DMT99_QUANTITY_FARAD;scale=1e-6;break;
    case 17:kind=DMT99_QUANTITY_FARAD;scale=1e-3;break;
    case 18:kind=DMT99_QUANTITY_HERTZ;scale=1.0;break;
    case 19:kind=DMT99_QUANTITY_HERTZ;scale=1e3;break;
    case 20:kind=DMT99_QUANTITY_HERTZ;scale=1e6;break;
    default:return false;
    }
    *out=((double)r->mantissa/r->divisor)*scale;
    *quantity=kind;
    return true;
}

bool dmt99_resistance_ohms(const dmt99_rx *r, double *out)
{
    double value;dmt99_quantity kind;
    if (!out || !dmt99_value_si(r,&value,&kind) || kind!=DMT99_QUANTITY_OHM) return false;
    *out=value;
    return true;
}

bool dmt99_stream_init(dmt99_stream *s, uint8_t display_bytes, dmt99_record_callback cb, void *ctx)
{
    if (!s || !cb || (display_bytes!=12 && display_bytes!=13)) return false;
    memset(s,0,sizeof *s);s->display_bytes=display_bytes;s->callback=cb;s->context=ctx;
    return true;
}

static void emit(dmt99_stream *s, uint8_t n, dmt99_record_kind kind)
{
    dmt99_record r;
    memset(&r,0,sizeof r);r.kind=kind;r.length=n;
    memcpy(r.raw,s->raw,n);memcpy(r.uart_errors,s->errors,n);
    if (kind==DMT99_RECORD_FRAME) (void)dmt99_parse_rx(r.raw,n,&r.parsed);
    s->callback(s->context,&r);
    s->count-=n;
    memmove(s->raw,s->raw+n,s->count);memmove(s->errors,s->errors+n,s->count);
}

void dmt99_stream_feed(dmt99_stream *s, uint8_t byte, uint8_t error)
{
    if (!s || !s->callback) return;
    s->raw[s->count]=byte;s->errors[s->count]=error;++s->count;
    while (s->count) {
        uint8_t expected=0;
        if (s->raw[0]!=0x5a && s->raw[0]!=0xaa) {emit(s,1,DMT99_RECORD_UNFRAMED);continue;}
        if (s->count<2) return;
        if (s->raw[0]==0x5a && s->raw[1]==0xa5) expected=s->display_bytes;
        else if (s->raw[0]==0xaa && s->raw[1]==0x55) {
            if (s->count<3) return;
            if (s->raw[2]==0xfd) expected=10;
        }
        if (!expected) {emit(s,1,DMT99_RECORD_UNFRAMED);continue;}
        if (s->count<expected) return;
        emit(s,expected,DMT99_RECORD_FRAME);
    }
}

void dmt99_stream_idle(dmt99_stream *s)
{
    if (s && s->callback && s->count) emit(s,s->count,DMT99_RECORD_INCOMPLETE);
}
