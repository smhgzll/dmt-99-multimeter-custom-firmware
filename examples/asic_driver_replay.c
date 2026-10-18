/* Host-only usage example. The callback is a mock UART; no physical IO.
 * A WCH application may bind recovered_asic_transmit after its own verified
 * board initialization. No display, filesystem or application controller links. */
#include "asic_driver.h"
#include <stdio.h>
static bool transmit(void *context,const uint8_t packet[10])
{
    (void)context;printf("MCU -> peer:");
    for(unsigned i=0;i<10;i++)printf(" %02X",packet[i]);
    putchar('\n');return true;
}
int main(void)
{
    dmt_asic_io uart={.transmit=transmit};
    if(!asic_request_mode(&uart,0x0c))return 1;
    /* A local transmit success does not establish an ASIC mode change. */
    const uint8_t event[13]={0x5a,0xa5,0xfd,0x0c};measurement_frame frame;
    if(asic_receive_idle(event,sizeof event,&frame)!=MEASUREMENT_EVENT_FRAME)return 1;
    printf("Explicit fixture event: %02X (correlation/acknowledgement unproven)\n",frame.event);
    const uint8_t packet[13]={0x5a,0xa5,1,0xaa,0x9d,0x4f,0x0e,0,2,0,0,0,0};
    measurement_reading reading={.mode=frame.event};
    if(!asic_reading_update(&reading,packet,sizeof packet))return 1;
    printf("Fixture LCD text: %s; host unit code: %u; value: %.3f\n",reading.text,reading.unit,(double)reading.value);
    return 0;
}
