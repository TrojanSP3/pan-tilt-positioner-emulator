#ifndef PROTOCOL_MSO2_H
#define PROTOCOL_MSO2_H

#include <unordered_map>

#include "protocol_base.h"

class Protocol_MSO2: public Protocol_Base
{
public:
    void PeriodLoop();
    virtual ~Protocol_MSO2();
    static const int NUMBER_OF_INPUTS = 16;
protected:
    virtual void ProtocolInit();
    virtual void ProtocolTick();
    virtual Protocol_Base::Command RecognizeCommand(const std::string command) const;
protected:
    void Process_azimuth_synchronization();
    void Process_elevation_synchronization();
    static void period_thread_procedure(Protocol_MSO2* obj);

    bool isProcessing_azimuth_synchronization = false;
    bool isProcessing_elevation_synchronization = false;
    bool isProcessing_azimuth_positioning = false;
    int processing_azimuth_positioning_target;
    bool isProcessing_elevation_positioning = false;
    int processing_elevation_positioning_target;

    const int MIN_PERIOD_INTERVAL_MS = 10;
    std::atomic<bool> need_to_destroy_thread = {false};
    std::thread* thread_period = nullptr;
    std::atomic<bool> need_to_send_period = {false};
    std::atomic<int> period_intervals_by_10_ms = {0};
private:
    void CreateCommandMap();
    void ProcessTcpIncomingData();
    void Cmd_a_pos(const Command& command);
    void Cmd_e_pos(const Command& command);
    void Cmd_a_vel(const Command& command);
    void Cmd_e_vel(const Command& command);
    void Cmd_e_smin(const Command& command);
    void Cmd_e_smax(const Command& command);
    void Cmd_i_set(const Command& command);
    void Cmd_o_set(const Command& command);
    void Cmd_reset();
    void Cmd_period(const Command& command);
    void Send_period();
    std::string GetCommandString();
    std::unordered_map<std::string, CommandType> CommandMap;
};

#endif // PROTOCOL_MSO2_H
