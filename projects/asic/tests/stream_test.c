#include "dmt99_asic.h"
#include <assert.h>
#include <string.h>

static uint8_t original[100000], errors[100000];
static size_t emitted;
static unsigned frames, events, display;
static void record(void *unused, const dmt99_record *r)
{
    (void)unused;
    assert(r->length && r->length<=13);
    assert(emitted+r->length<=sizeof original);
    assert(memcmp(r->raw,original+emitted,r->length)==0);
    assert(memcmp(r->uart_errors,errors+emitted,r->length)==0);
    if (r->kind==DMT99_RECORD_FRAME) {
        ++frames;
        assert(r->parsed.length==r->length);
        assert(r->parsed.kind!=DMT99_RX_UNKNOWN);
        if (r->parsed.kind==DMT99_RX_DISPLAY) ++display;
        if (r->parsed.kind==DMT99_RX_EVENT) ++events;
    }
    emitted+=r->length;
}

int main(void)
{
    static const uint8_t observed[]={
        0x11,0x5a,0xaa, /* deliberate prefix noise */
        0x5a,0xa5,0xe4,0x2e,0x63,0x25,7,0,0,0,1,0x3f,
        0xaa,0x55,0xfd,0x14,0,0,0,0xaa,1,0xbb,
        0x5a,0xa5,0xe4 /* deliberate trailing fragment */
    };
    dmt99_stream s;
    assert(!dmt99_stream_init(&s,11,record,0));
    assert(!dmt99_stream_init(&s,12,0,0));
    assert(dmt99_stream_init(&s,12,record,0));
    memcpy(original,observed,sizeof observed);
    for (size_t i=0;i<sizeof observed;++i) {
        errors[i]=(uint8_t)(i%5);
        dmt99_stream_feed(&s,original[i],errors[i]);
    }
    dmt99_stream_idle(&s);
    assert(emitted==sizeof observed && frames==2 && events==1 && display==1);
    uint32_t random_state=99;
    emitted=0;
    assert(dmt99_stream_init(&s,13,record,0));
    for (size_t i=0;i<sizeof original;++i) {
        random_state=random_state*1664525u+1013904223u;
        original[i]=(uint8_t)(random_state>>24);
        errors[i]=(uint8_t)(random_state&3);
        dmt99_stream_feed(&s,original[i],errors[i]);
        if (i%31==0) dmt99_stream_idle(&s);
    }
    dmt99_stream_idle(&s);
    assert(emitted==sizeof original);
    return 0;
}
