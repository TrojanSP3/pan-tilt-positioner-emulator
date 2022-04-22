#ifndef TESTTCPSERVER_H
#define TESTTCPSERVER_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestTcpServer: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    protected:
        //static void StartAndInfinityListen();
        static void Start_Stop_IsOpen();
        static void Start_Restart();
        static void Port();
        static void Send();
        static void Recv_IsDataAvailable();
        static void Recv_3lines();
        static void ClientChanging();
        static void SeveralClients();
        static void Start_Restart_With_Client();
    private:
        static const int LONG_TIMEOUT;
    };

}
#endif // TESTTCPSERVER_H
