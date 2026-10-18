/* Recovered from 0x97c4, 0x97f8, 0x98aa, 0x997e, 0xbe78, 0xc468.
 * This decodes the CH32-facing protocol; it does not identify the ASIC or its ADC registers.
 * TODO(0xbe78/0xc468): determine bytes 9..12 and any incoming integrity rule from additional evidence.
 */
#include "measurement_asic.h"

uint8_t measurement_segment_decode(uint8_t segments)
{
    static const uint8_t patterns[] = {
        0xeb,0x0a,0xad,0x8f,0x4e,0xc7,0xe7,0x8a,0xef,0xcf,
        0xee,0x23,0x65,0x27,0x61,0xe5,0x04,0x00,0xe1,0xec
    };
    segments &= 0xef;
    for (uint8_t i=0; i<sizeof patterns; ++i)
        if (patterns[i] == segments) return i;
    if (segments == 0x24) return 0x15;
    return 0xff;
}

bool measurement_build_command(uint8_t command, uint8_t out[MEASUREMENT_TX_BYTES])
{
    /* Only command IDs actually emitted by 0x97f8/0xb5c8/0xb9e0/0xbc90.
     * For these zero-payload commands byte 9 is command+5. No general checksum
     * algorithm for arbitrary payloads or incoming frames is asserted.
     */
    if (!(command == 1 || command == 3 || command == 8 || command == 9 ||
          (command >= 0x0a && command <= 0x16))) return false;
    for (size_t i=0; i<MEASUREMENT_TX_BYTES; ++i) out[i]=0;
    out[0]=0xaa; out[1]=0x55; out[2]=5; out[3]=command; out[9]=(uint8_t)(command+5);
    return true;
}
void measurement_forward_button(uint8_t *event,const dmt_button_io *o,void *ctx,void (*send)(void *,const uint8_t[10]))
{
    static const uint16_t pins[]={0x400,0x800,0x200,0x1000};
    static const uint8_t commands[]={9,8,3,1};
    uint8_t key=*event;
    if(key<1 || key>4)return;
    o->delay_ms(o->context,10);
    if(!o->pin_high(o->context,pins[key-1])){
        uint8_t packet[10];measurement_build_command(commands[key-1],packet);send(ctx,packet);
    }
    *event=0;
}
void measurement_diagnostic_entry(void *ctx,void (*send)(void *,const uint8_t[10]),void (*delay)(void *,uint32_t))
{
    uint8_t packet[10]={0xaa,0x55,5,0x0b,0,0,0,0,0,0x10};
    send(ctx,packet);delay(ctx,200);
    packet[2]=2;packet[3]=0xa0;packet[9]=0xa2;send(ctx,packet);
}

bool measurement_frame_is_blank(const uint8_t *b)
{
    /* Exact suppression predicate in USART1_IRQHandler at 0xc468. */
    return !(b[2]&0xe0) && !(b[3]&0x0f) && !(b[3]&0xe0) &&
           !(b[4]&0x0f) && !(b[4]&0xe0) && !(b[5]&0x0f) &&
           !(b[5]&0xe0) && !(b[6]&0x0f);
}

void measurement_capture_idle(measurement_rx_state *state, const uint8_t *frame)
{
    state->suppress_measurement=measurement_frame_is_blank(frame);
    if (frame[2] == 0xfd) {
        for (size_t i=0; i<MEASUREMENT_RX_BYTES; ++i) state->saved_event[i]=frame[i];
        state->event_pending=true;
    }
}

static uint8_t unit_code(const uint8_t *b)
{
    /* 0xbe78: preserve original unit IDs, including otherwise unnamed codes. */
    if (b[8]&2) return (b[7]&1) ? 7 : 6;
    if (b[7]&0x20) return (b[6]&0x40) ? 12 : (b[7]&4) ? 13 : 11;
    if (b[8]&1) return (b[6]&0x10) ? 8 : (b[7]&1) ? 9 : 10;
    if (b[7]&0x10) return (b[6]&0x20) ? 15 : (b[6]&0x10) ? 16 : 17;
    if (b[8]&0x20) return 21;
    if (b[8]&4) return (b[6]&0x40) ? 19 : (b[7]&4) ? 20 : 18;
    return 1;
}

measurement_frame_kind measurement_decode_frame(const uint8_t *b, size_t length, measurement_frame *out)
{
    /* Explicit table below avoids assigning character encodings from a guessed alphabet. */
    static const char recovered[]={'0','1','2','3','4','5','6','7','8','9',
        'A','U','T','O','L','E','-',' ','C','P','S','r'};
    if (!b || !out || length != MEASUREMENT_RX_BYTES) return MEASUREMENT_INVALID;
    *out=(measurement_frame){0};
    for (size_t i=0; i<MEASUREMENT_RX_BYTES; ++i) out->raw[i]=b[i];
    if (b[0]!=0x5a || b[1]!=0xa5) return MEASUREMENT_INVALID;
    if (b[2]==0xfd) {
        out->event=b[3]; out->kind=MEASUREMENT_EVENT_FRAME; return out->kind;
    }
    for (size_t i=0; i<4; ++i) {
        uint8_t v=measurement_segment_decode((uint8_t)((b[2+i]&0xf0)|(b[3+i]&15)));
        out->symbol_codes[i]=v;
        out->symbols[i]=(v<sizeof recovered) ? recovered[v] : 'X';
    }
    out->negative=(b[2]&0x10)!=0;
    out->decimal_after=(b[3]&0x10) ? 1 : (b[4]&0x10) ? 2 : (b[5]&0x10) ? 3 : 0;
    out->ac_dc_code=(b[2]&2) ? 2 : (b[2]&1) ? 1 : 255;
    out->unit_code=unit_code(b);
    out->high_status_bit=(b[7]&0x80)!=0;
    out->kind=MEASUREMENT_DISPLAY_FRAME;
    return out->kind;
}
