#include "mso_state.h"

bool MSO_State::isChanged(const MSO_State& other)
{
    if(this->azimuth_position!=other.azimuth_position)
        return true;
    if(this->azimuth_speed!=other.azimuth_speed)
        return true;
    if(this->azimuth_syncronized!=other.azimuth_syncronized)
        return true;
    if(this->elevation_position!=other.elevation_position)
        return true;
    if(this->elevation_speed!=other.elevation_speed)
        return true;
    if(this->elevation_syncronized!=other.elevation_syncronized)
        return true;
    if(this->inputs!=other.inputs)
        return true;
    if(this->outputs!=other.outputs)
        return true;
    return false;
}
