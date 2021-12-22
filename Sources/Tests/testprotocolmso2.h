#ifndef TESTPROTOCOLMSO2_H
#define TESTPROTOCOLMSO2_H

#include "Framework/testframework.h"

#include "../protocol_mso2.h"
namespace UnitTests {

    class TestProtocolMSO2: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void Create_Destroy();
        static void Init();
        static void Init_ticks();
        static void Client_connected();
        static void Info_and_Voltage();
        static void Azimuth_syn();
        static void Azimuth_syn_fast();
        static void Elevation_syn();
        static void Elevation_syn_fast();
        static void Positioning();
        static void Speed();
        static void Elevation_limits();
        static void Inputs_outputs();
        static void Period();
        static void Reset();
    private:
        static void Inputs_outputs_init_1(const int port);
        static void Inputs_outputs_init_2(const int port);
        static void Inputs_outputs_check_input(
                Protocol_MSO2* const protocol, TcpClientSocket* const client,
                const size_t num);
        static void Inputs_outputs_check_input_set_get_assert(
                Protocol_MSO2* const protocol, TcpClientSocket* const client,
                const size_t num, const bool value);
        static void Inputs_outputs_check_output(
                Protocol_MSO2* const protocol, TcpClientSocket* const client,
                const size_t num);
        static void Inputs_outputs_check_output_set_get_assert(
                Protocol_MSO2* const protocol, TcpClientSocket* const client,
                const size_t num, const bool value);
        static MSO_State ParsePeriodLine(std::string period_line);
    private:
        static const int SHORT_SLEEP;
        static const int LONG_TIMEOUT;
        static const uint16_t BASE_PORT;
        static uint16_t PORT_CNT;
    };

}

#endif // TESTPROTOCOLMSO2_H
