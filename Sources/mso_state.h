#ifndef MSO_STATE_H
#define MSO_STATE_H

#include <stdint.h>
#include <string>
#include <vector>

struct MSO_State
{
	MSO_State();

    int64_t timestamp;

    int azimuth_position;
    int azimuth_speed;
    bool azimuth_syncronized;

    int elevation_position;
    int elevation_speed;
    bool elevation_syncronized;

    std::vector<bool> inputs;
    std::vector<bool> outputs;

    int voltage;

    std::string tcp_last_got_data;
    std::string tcp_last_sent_data;

    bool isChanged(const MSO_State& other);
};


#endif // MSO_STATE_H
