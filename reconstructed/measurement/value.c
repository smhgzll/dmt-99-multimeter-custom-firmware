/* Controller adapter for the shared original LCD parser. */
#include "control.h"
bool control_parse(dmt_control *s,const uint8_t b[13])
{
    measurement_reading r={.mode=s->mode,.numeric=s->numeric};
    for(unsigned i=0;i<sizeof r.text;i++)r.text[i]=s->text[i];
    if(!measurement_parse_reading(&r,b))return false;
    s->numeric=r.numeric;s->unit=r.unit;s->ac_dc_code=r.ac_dc_code;s->status_high=r.status_high;s->value=r.value;
    for(unsigned i=0;i<sizeof s->text;i++)s->text[i]=r.text[i];
    return true;
}
float measurement_relative_value(const dmt_control *s)
{ return s->relative?s->value-s->relative_value:s->value; }
