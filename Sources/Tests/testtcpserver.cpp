#include "testtcpserver.h"

#include "../tcpserver.h"

using namespace UnitTests;

const int TestTcpServer::LONG_TIMEOUT=5000;
const std::string LOCALHOST = "127.0.0.1";
std::vector<TestCase> TestTcpServer::GetTestCases()
{
    const std::string LOGMODULE="TestTcpServer";
    std::vector<TestCase> result;

    result.push_back(TESTCASE(Start_Stop_IsOpen));
    result.push_back(TESTCASE(Start_Restart));
    result.push_back(TESTCASE(Port));
    result.push_back(TESTCASE(Send));
    result.push_back(TESTCASE(Recv_IsDataAvailable));
    result.push_back(TESTCASE(ClientChanging));
    result.push_back(TESTCASE(Recv_3lines));
    result.push_back(TESTCASE(SeveralClients));
    result.push_back(TESTCASE(Start_Restart_With_Client));
    return result;
}

void TestTcpServer::Start_Stop_IsOpen()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");
    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened");
}

void TestTcpServer::Start_Restart()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");
    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened");

    TestFramework::LogMsg("Re-opening the server");
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed 2");
    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened 2");
}

void TestTcpServer::Port()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TCPServer server;
    ASSERT(server.Port()==0);
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");
    ASSERT(server.Port()==PORT);
    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened");
    ASSERT(server.Port()==0);
}

void TestTcpServer::Send()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    const std::string TESTSTRING = "test string";
    const std::string TESTMESSAGE = TESTSTRING+"\n";

    TestFramework::LogMsg("Starting server...");
    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    TestFramework::LogMsg("Opening client...");
    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TestFramework::LogMsg("Waiting for a client...");
    TIMEOUT(server.IsClientPresent(),LONG_TIMEOUT);

    TestFramework::LogMsg("Sending message...");
    server.Send(TESTMESSAGE);

    TestFramework::LogMsg("Reading a message...");
    TIMEOUT(client.BytesAvailable(),LONG_TIMEOUT);
    std::string recvstring = client.ReadLine();

    TestFramework::LogMsg("Stopping a server...");
    server.Stop();
    client.Close();

    TestFramework::LogMsg("Comparing results...");
    if(recvstring.compare(TESTSTRING))
        EXCEPTION("Different strings: "+TESTSTRING+" != "+recvstring);
}

void TestTcpServer::Recv_IsDataAvailable()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    const std::string TESTSTRING = "te5t 5tr1ng";
    const std::string MESSAGE = TESTSTRING+"\n";
    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    TestFramework::LogMsg("Opening client...");
    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TestFramework::LogMsg("Waiting for a client...");
    TIMEOUT(server.IsClientPresent(),LONG_TIMEOUT);

    if(server.IsDataAvailable())
        EXCEPTION("Data should not be available at this point!");

    client.WriteLine(MESSAGE);

    TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);

    std::string recvstring = server.Recv();
    if(recvstring.compare(TESTSTRING))
        EXCEPTION("Different strings: "+TESTSTRING+" != "+recvstring);
    client.Close();
    server.Stop();
}

void TestTcpServer::ClientChanging()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    const std::string TESTSTRING_TOCLIENT = "tst asd";
    const std::string MESSAGE_TOCLIENT = TESTSTRING_TOCLIENT+"\n";
    const std::string TESTSTRING_TOSERVER = "sm tst";
    const std::string MESSAGE_TOSERVER = TESTSTRING_TOSERVER+"\n";
    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    for(int i=1; i<=5; ++i)
    {
        TestFramework::LogMsg("Iteration: "+std::to_string(i));
        TestFramework::LogMsg("Opening client...");
        TcpClientSocket client;
        client.Open(LOCALHOST,PORT);
        ASSERT(client.IsOpen());
        TIMEOUT(server.IsClientPresent(),LONG_TIMEOUT);
        if(server.IsDataAvailable())
            EXCEPTION("Data should not be available at this point!");

        TestFramework::LogMsg("Server read...");
        client.WriteLine(MESSAGE_TOSERVER);
        TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);
        std::string recvstring = server.Recv();
        if(recvstring.compare(TESTSTRING_TOSERVER))
            EXCEPTION("Different strings: "+TESTSTRING_TOSERVER+" != "+recvstring);

        TestFramework::LogMsg("Client read...");
        server.Send(MESSAGE_TOCLIENT);
        TIMEOUT(client.BytesAvailable(),LONG_TIMEOUT);
        recvstring = client.ReadLine();
        if(recvstring.compare(TESTSTRING_TOCLIENT))
            EXCEPTION("Different strings: "+TESTSTRING_TOCLIENT+" != "+recvstring);

        TestFramework::LogMsg("Client closing...");
        client.Close();
        ASSERT(!client.IsOpen());
        TIMEOUT(!server.IsClientPresent(),LONG_TIMEOUT);
    }

    server.Stop();
}

void TestTcpServer::Recv_3lines()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    const std::string TESTSTR_1 = "test1 str1";
    const std::string TESTSTR_2 = "test2 str2";
    const std::string TESTSTR_3 = "test3 str3";
    const std::string ENDL = "\n";

    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());

    client.WriteLine(TESTSTR_1+ENDL);
    client.WriteLine(TESTSTR_2+ENDL);
    client.WriteLine(TESTSTR_3+ENDL);
    std::string recvstring = "";

    TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);
    recvstring = server.Recv();
    if(recvstring.compare(TESTSTR_1))
        EXCEPTION("Different strings: "+TESTSTR_1+" != "+recvstring);

    TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);
    recvstring = server.Recv();
    if(recvstring.compare(TESTSTR_2))
        EXCEPTION("Different strings: "+TESTSTR_2+" != "+recvstring);

    TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);
    recvstring = server.Recv();
    if(recvstring.compare(TESTSTR_3))
        EXCEPTION("Different strings: "+TESTSTR_3+" != "+recvstring);

    server.Stop();
    client.Close();
}

void TestTcpServer::SeveralClients()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    const std::string TESTSTR_1 = "test1 str1\n";
    const std::string TESTSTR_2 = "test2 str2\n";
    const std::string TESTSTR_3 = "test3 str3\n";
    const std::string TESTSTR_4 = "test4 str4\n";

    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    TcpClientSocket client_1;
    client_1.Open(LOCALHOST,PORT);
    ASSERT(client_1.IsOpen());

    SLEEPMS(300);
    TcpClientSocket client_2;
    client_2.Open(LOCALHOST,PORT);

    TcpClientSocket client_3;
    client_3.Open(LOCALHOST,PORT);

    client_1.WriteLine(TESTSTR_1);
    client_2.WriteLine(TESTSTR_2);
    client_3.WriteLine(TESTSTR_3);

    TIMEOUT(server.IsDataAvailable(), LONG_TIMEOUT);
    std::string lineFromClient1 = server.Recv();
    TIMEOUT(server.IsDataAvailable(), LONG_TIMEOUT);
    std::string lineFromClient2 = server.Recv();
    TIMEOUT(server.IsDataAvailable(), LONG_TIMEOUT);
    std::string lineFromClient3 = server.Recv();

    ASSERT(lineFromClient1!=lineFromClient2);
    ASSERT(lineFromClient1!=lineFromClient3);
    ASSERT(lineFromClient2!=lineFromClient3);

    lineFromClient1+="\n";
    lineFromClient2+="\n";
    lineFromClient3+="\n";

    std::string tmp_compare;
    tmp_compare = TESTSTR_1;
    bool getLineFromClient1 =
            lineFromClient1 == tmp_compare
            || lineFromClient2 == tmp_compare
            || lineFromClient3 == tmp_compare;

    tmp_compare = TESTSTR_2;
    bool getLineFromClient2 =
            lineFromClient1 == tmp_compare
            || lineFromClient2 == tmp_compare
            || lineFromClient3 == tmp_compare;

    tmp_compare = TESTSTR_3;
    bool getLineFromClient3 =
            lineFromClient1 == tmp_compare
            || lineFromClient2 == tmp_compare
            || lineFromClient3 == tmp_compare;

    ASSERT(getLineFromClient1);
    ASSERT(getLineFromClient2);
    ASSERT(getLineFromClient3);

    ASSERT(!server.IsDataAvailable());

    server.Send(TESTSTR_4);

    TIMEOUT(client_1.BytesAvailable()>0, LONG_TIMEOUT);
    tmp_compare = client_1.ReadLine();
    tmp_compare += "\n";
    ASSERT(tmp_compare == TESTSTR_4);
    ASSERT(client_1.BytesAvailable()==0);

    TIMEOUT(client_2.BytesAvailable()>0, LONG_TIMEOUT);
    tmp_compare = client_2.ReadLine();
    tmp_compare += "\n";
    ASSERT(tmp_compare == TESTSTR_4);
    ASSERT(client_2.BytesAvailable()==0);

    TIMEOUT(client_3.BytesAvailable()>0, LONG_TIMEOUT);
    tmp_compare = client_3.ReadLine();
    tmp_compare += "\n";
    ASSERT(tmp_compare == TESTSTR_4);
    ASSERT(client_3.BytesAvailable()==0);

    client_1.Close();
    client_2.Close();
    client_3.Close();
    server.Stop();
}

void TestTcpServer::Start_Restart_With_Client()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    TcpClientSocket client;

    TCPServer server;
    server.Start(PORT);
    if(!server.IsOpen())
        EXCEPTION("Server closed");

    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TIMEOUT(server.IsClientPresent(),LONG_TIMEOUT);
    client.WriteLine("TESTSTR_1\r\n");
    TIMEOUT(server.IsDataAvailable(),LONG_TIMEOUT);

    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened");
    TIMEOUT(!client.IsOpen(),LONG_TIMEOUT);

    TestFramework::LogMsg("Re-opening the server");
    const uint16_t PORT_NEXT = TestUtilities::TCP_GetFreePort();
    server.Start(PORT_NEXT); ;
    if(!server.IsOpen())
        EXCEPTION("Server closed 2");
    server.Stop();
    if(server.IsOpen())
        EXCEPTION("Server opened 2");
}
