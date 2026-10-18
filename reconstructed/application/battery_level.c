/* Native 75f0 battery estimate. Machine-code comparison of all operations:
 * tools/test_battery_differential.py. */
#include "battery_level.h"
int battery_level_percent(uint16_t raw,float *millivolts,float *level)
{
    float mv=(float)(int32_t)(((uint32_t)raw*0x19c8u)>>12);
    float l=1.0f-(4000.0f-mv)/600.0f;
    if(millivolts)*millivolts=mv;
    if(level)*level=l;
    return (int)(l*100.0f);
}
