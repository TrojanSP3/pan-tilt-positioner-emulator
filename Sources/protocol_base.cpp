#include "protocol_base.h"

Protocol_Base::Command::Command()
{
	command=CommandType::UNKNOWN;
}

void Protocol_Base::Init(const MSO_State init_state,int tcp_server_port)
{
    state = init_state;
    engine.Start();
    engine.SetAzimuthPosition(state.azimuth_position);
    engine.SetElevationPosition(state.elevation_position);
    int counter = 0;
    while( engine.GetAzimuthPosition()!=state.azimuth_position || engine.GetElevationPosition()!=state.elevation_position )
    {
        counter++;
        if(counter>100*100)
            throw std::runtime_error("Protocol_Base: Init error");
        Utils::Sleep(100);
    }
    if(state.azimuth_speed!=0)
        engine.SetAzimuthSpeed(state.azimuth_speed);
    if(state.elevation_speed!=0)
        engine.SetElevationSpeed(state.elevation_speed);
    tcp_server.Start(tcp_server_port);

    ProtocolInit();
}

MSO_State Protocol_Base::Tick()
{
    ProtocolTick();
    return state;
}

Protocol_Base::~Protocol_Base()
{
    engine.Stop();
    tcp_server.Stop();
}
