#ifndef PROTOCOL_MSO2_H
#define PROTOCOL_MSO2_H
#include "protocol_base.h"

#include <unordered_map>

class Protocol_MSO2: public Protocol_Base
{
public:
    void PeriodLoop();
	Protocol_MSO2();
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

    bool isProcessing_azimuth_synchronization;
    bool isProcessing_elevation_synchronization;
    bool isProcessing_azimuth_positioning;
    int processing_azimuth_positioning_target;
    bool isProcessing_elevation_positioning;
    int processing_elevation_positioning_target;

    static const int MIN_PERIOD_INTERVAL_MS = 10;
    std::atomic<bool> need_to_destroy_thread;
    std::thread* thread_period;
    std::atomic<bool> need_to_send_period;
    std::atomic<int> period_intervals_by_10_ms;
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
