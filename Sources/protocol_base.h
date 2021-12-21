#ifndef PROTOCOL_BASE_H
#define PROTOCOL_BASE_H

#include <vector>
#include <string>

#include "mso_state.h"
#include "engine.h"
#include "tcpserver.h"

class Protocol_Base
{
public:
    void Init(const MSO_State init_state, int tcp_server_port);
    MSO_State Tick();
    virtual ~Protocol_Base();
protected:
    virtual void ProtocolInit()=0;
    virtual void ProtocolTick()=0;

    MSO_State state;
    Engine engine;
    TCPServer tcp_server;
protected:
    enum CommandType
    {
        UNKNOWN,
        AZIMUTH_SET_SYNCHRONIZATION,
        ELEVATION_SET_SYNCHRONIZATION,
        AZIMUTH_GET_SYNCHRONIZATION,
        ELEVATION_GET_SYNCHRONIZATION,
        AZIMUTH_SET_POSITION,
        ELEVATION_SET_POSITION,
        AZIMUTH_GET_POSITION,
        ELEVATION_GET_POSITION,
        AZIMUTH_SET_VELOCITY,
        ELEVATION_SET_VELOCITY,
        AZIMUTH_GET_VELOCITY,
        ELEVATION_GET_VELOCITY,
        ELEVATION_SET_MINIMUM_POSITION,
        ELEVATION_GET_MINIMUM_POSITION,
        ELEVATION_SET_MAXIMUM_POSITION,
        ELEVATION_GET_MAXIMUM_POSITION,
        AZIMUTH_GET_VOLTAGE,
        ELEVATION_GET_VOLTAGE,
        SERIAL_SET_MODE,
        SERIAL_SEND_DATA,
        INPUTS_SET,
        INPUTS_GET,
        OUTPUTS_SET,
        OUTPUTS_GET,
        RESET,
        PERIOD,
        INFO
    };
    struct Command
    {
        CommandType command=CommandType::UNKNOWN;
        std::vector<std::string> parameters;
    };
    virtual Protocol_Base::Command RecognizeCommand(const std::string command) const = 0;

};

#endif // PROTOCOL_BASE_H
