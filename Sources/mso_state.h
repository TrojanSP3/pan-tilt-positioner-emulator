#ifndef MSO_STATE_H
#define MSO_STATE_H

#include <string>
#include <vector>
#include <atomic>

struct MSO_State
{
    int64_t timestamp;

    int azimuth_position=0;
    int azimuth_speed=0;
    bool azimuth_syncronized = false;

    int elevation_position=0;
    int elevation_speed=0;
    bool elevation_syncronized = false;

    std::vector<bool> inputs;
    std::vector<bool> outputs;

    int voltage=0;

    std::string tcp_last_got_data = "";
    std::string tcp_last_sent_data = "";

    bool isChanged(const MSO_State& other);
};


#endif // MSO_STATE_H
