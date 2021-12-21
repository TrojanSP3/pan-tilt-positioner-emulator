#ifndef TESTTCPSERVERSOCKET_H
#define TESTTCPSERVERSOCKET_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestTcpSocket: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    protected:
        static void StartAndInfinityListen();
        static void Start_IsOpen_Stop();
        static void Start_Restart();
        static void SamePort();
        static void Rebind();
        static void ClientWrongAddress();
        static void GetClient();
        static void DropClient();
        static void WriteToClient_raw();
        static void WriteToClient_line();
        static void ReadFromClient_raw();
        static void ReadFromClient_line();
    private:
        static const int LONG_TIMEOUT;
        static const int BASE_PORT;
        static int PORT_CNT;
    };

}

#endif // TESTTCPSERVERSOCKET_H
