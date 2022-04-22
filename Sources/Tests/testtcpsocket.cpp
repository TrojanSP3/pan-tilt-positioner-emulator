#include "testtcpsocket.h"

#include <cstring>

#include "../TcpSocket/tcpserversocket.h"
#include "../TcpSocket/tcpclientsocket.h"


using namespace UnitTests;

const int TestTcpSocket::LONG_TIMEOUT=5000;

const std::string LOCALHOST = "127.0.0.1";

std::vector<TestCase> TestTcpSocket::GetTestCases()
{
    const std::string LOGMODULE="TestTcpSocket";
    std::vector<TestCase> result;
    //result.push_back(TESTCASE(StartAndInfinityListen));

    result.push_back(TESTCASE(Start_IsOpen_Stop));
    result.push_back(TESTCASE(Start_Restart));
    result.push_back(TESTCASE(SamePort));
    result.push_back(TESTCASE(Rebind));
    result.push_back(TESTCASE(ClientWrongAddress));
    result.push_back(TESTCASE(GetClient));
    result.push_back(TESTCASE(DropClient));
    result.push_back(TESTCASE(WriteToClient_raw));
    result.push_back(TESTCASE(WriteToClient_line));
    result.push_back(TESTCASE(ReadFromClient_raw));
    result.push_back(TESTCASE(ReadFromClient_line));
    return result;
}

void TestTcpSocket::StartAndInfinityListen()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TestFramework::LogMsg(">>>Infinity listening test<<<");
    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());
    TestFramework::LogMsg(">Server started at port: "+std::to_string(PORT));
    while(server.IsListening())
    {
        if(server.IsClientAvailable())
        {
            TestFramework::LogMsg(">Getting new client...");
            std::shared_ptr<TcpClientSocket> client= server.AcceptClient();
            TestFramework::LogMsg(">Ready to read");
            while(client->IsOpen())
            {
                if(client->BytesAvailable())
                {
                    std::string in = client->ReadLine();
                    if(!in.empty())
                    {
                        TestFramework::LogMsg(">Get: "+in);
                        client->WriteLine("Get it!\n");
                    }
                }
                SLEEPMS(100);
            }
            TestFramework::LogMsg(">Client lost...");
            client->Close();
        }
        SLEEPMS(100);
    }
    TestFramework::LogMsg(">Server STOPPED!");
    server.Stop();
}

void TestTcpSocket::Start_IsOpen_Stop()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());
    server.Stop();
    ASSERT(!server.IsListening());
}

void TestTcpSocket::Start_Restart()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());
    server.Stop();
    ASSERT(!server.IsListening());
    TestFramework::LogMsg("Re-opening the server");
    server.Start(PORT);
    ASSERT(server.IsListening());
    server.Stop();
    ASSERT(!server.IsListening());
}

void TestTcpSocket::SamePort()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TcpServerSocket server_1;
    TcpServerSocket server_2;

    server_1.Start(PORT);
    ASSERT(server_1.IsListening());
    try
    {
        server_2.Start(PORT);
        EXCEPTION("Exception must be thrown here");
    }
    catch(TcpSocketException& ex)
    {
        const std::string substr= "98: Address already in use";
        std::string msg=ex.what();
        if(msg.find(substr)==std::string::npos)
        {
            TestFramework::LogMsg("Wrong exception");
            throw;
        }
    }

    server_1.Stop();
    server_2.Stop();
}

void TestTcpSocket::Rebind()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();
    TcpServerSocket server_1;
    TcpServerSocket server_2;
    TcpServerSocket server_3;

    server_1.Start(PORT);
    ASSERT(server_1.IsListening());
    try
    {
        server_2.Start(PORT);
        EXCEPTION("Exception must be thrown here");
    }
    catch(TcpSocketException& ex)
    {
        const std::string substr= "98: Address already in use";
        std::string msg=ex.what();
        if(msg.find(substr)==std::string::npos)
            throw;
    }

    server_1.Stop();
    ASSERT(!server_1.IsListening());

    server_1.Start(PORT);
    ASSERT(server_1.IsListening());

    server_1.Stop();
    ASSERT(!server_1.IsListening());

    server_3.Start(PORT);
    ASSERT(server_3.IsListening());

    server_1.Stop();
    server_2.Stop();
    server_3.Stop();
}


void TestTcpSocket::ClientWrongAddress()
{
    const uint16_t GOOD_PORT = TestUtilities::TCP_GetFreePort();
    const uint16_t BAD_PORT = 0;
    const std::string BAD_PORT_MESSAGE = "port";
    const std::string GOOD_ADDRESS = "127.0.0.1";
    const std::string BAD_ADDRESS = "localhost";
    const std::string BAD_ADDRESS_MESSAGE = "address";

    TcpClientSocket client;
    try
    {
        client.Open(GOOD_ADDRESS,BAD_PORT);
    }
    catch(TcpSocketException& e)
    {
        std::string msg(e.what());
        ASSERT(msg.find(BAD_PORT_MESSAGE)!=std::string::npos);
    }

    try
    {
        client.Open(BAD_ADDRESS,GOOD_PORT);
    }
    catch(TcpSocketException& e)
    {
        std::string msg(e.what());
        ASSERT(msg.find(BAD_ADDRESS_MESSAGE)!=std::string::npos);
    }

    client.Close();

}

void TestTcpSocket::GetClient()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());

    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);
    std::shared_ptr<TcpClientSocket> client_get = server.AcceptClient();
    ASSERT(client_get->IsOpen());

    client.Close();
    TIMEOUT(!client.IsOpen(),LONG_TIMEOUT);
    TIMEOUT(!client_get->IsOpen(),LONG_TIMEOUT);
    client_get->Close();
    TIMEOUT(!client_get->IsOpen(),LONG_TIMEOUT);
    server.Stop();
}

void TestTcpSocket::DropClient()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    TcpServerSocket server;
    TestFramework::LogMsg("Trying to start server");
    server.Start(PORT);
    TestFramework::LogMsg("Server start procedure finished");
    ASSERT(server.IsListening());

    TcpClientSocket client1;
    client1.Open(LOCALHOST,PORT);
    ASSERT(client1.IsOpen());

    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);

    std::shared_ptr<TcpClientSocket> client1_get = server.AcceptClient();

    ASSERT(client1_get->IsOpen());

    client1.Close();

    TIMEOUT(!client1_get->IsOpen(),LONG_TIMEOUT);
    client1_get->Close();

    TIMEOUT(!server.IsClientAvailable(),LONG_TIMEOUT);

    TcpClientSocket client2;
    client2.Open(LOCALHOST,PORT);
    ASSERT(client2.IsOpen());

    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);

    std::shared_ptr<TcpClientSocket> client2_get = server.AcceptClient();

    ASSERT(client2_get->IsOpen());
    client2_get->Close();

    TIMEOUT(!client2.IsOpen(),LONG_TIMEOUT);

    client2.Close();

    server.Stop();
}

void TestTcpSocket::WriteToClient_raw()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    const std::string MESSAGE_DATA = "No end of line!";
    const size_t MESSAGE_LEN = 15;
    ASSERT(MESSAGE_LEN==MESSAGE_DATA.length());

    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);
    std::shared_ptr<TcpClientSocket> client_get = server.AcceptClient();
    ASSERT(client_get->IsOpen());

    client_get->Write(MESSAGE_DATA.c_str(),MESSAGE_LEN);
    TIMEOUT(static_cast<size_t>(client.BytesAvailable())==MESSAGE_LEN,LONG_TIMEOUT);
    char msgbuf[MESSAGE_LEN];
    memset(&msgbuf,0,MESSAGE_LEN);
    client.Read(msgbuf,MESSAGE_LEN);
    std::string msg(msgbuf);

    ASSERT(msg==MESSAGE_DATA);

    server.Stop();
    client.Close();
    client_get->Close();
}

void TestTcpSocket::WriteToClient_line()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    const std::string MESSAGE_DATA = "End of line ->";
    const std::string MESSAGE = MESSAGE_DATA+"\r\n";
    const size_t MESSAGE_LEN = MESSAGE.length();

    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);
    std::shared_ptr<TcpClientSocket> client_get = server.AcceptClient();
    ASSERT(client_get->IsOpen());

    client_get->WriteLine(MESSAGE);
    TIMEOUT(static_cast<size_t>(client.BytesAvailable())==MESSAGE_LEN,LONG_TIMEOUT);
    std::string msg = client.ReadLine();

    ASSERT(msg==MESSAGE_DATA);

    server.Stop();
    client.Close();
    client_get->Close();
}

void TestTcpSocket::ReadFromClient_raw()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    const std::string MESSAGE_DATA = "No end of line!";
    const size_t MESSAGE_LEN = 15;

    ASSERT(MESSAGE_LEN == MESSAGE_DATA.length());

    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);
    std::shared_ptr<TcpClientSocket> client_get = server.AcceptClient();
    ASSERT(client_get->IsOpen());

    client.Write(MESSAGE_DATA.c_str(),MESSAGE_LEN);
    TIMEOUT(static_cast<size_t>(client_get->BytesAvailable())==MESSAGE_LEN,LONG_TIMEOUT);
    char msgbuf[MESSAGE_LEN];
    client_get->Read(msgbuf,MESSAGE_LEN);
    std::string msg(msgbuf);

    ASSERT(msg==MESSAGE_DATA);

    server.Stop();
    client.Close();
    client_get->Close();
}

void TestTcpSocket::ReadFromClient_line()
{
    const uint16_t PORT = TestUtilities::TCP_GetFreePort();

    const std::string MESSAGE_DATA = "End of line ->";
    const std::string MESSAGE = MESSAGE_DATA+"\r\n";
    const size_t MESSAGE_LEN = MESSAGE.length();

    TcpServerSocket server;
    server.Start(PORT);
    ASSERT(server.IsListening());

    TcpClientSocket client;
    client.Open(LOCALHOST,PORT);
    ASSERT(client.IsOpen());
    TIMEOUT(server.IsClientAvailable(),LONG_TIMEOUT);
    std::shared_ptr<TcpClientSocket> client_get = server.AcceptClient();
    ASSERT(client_get->IsOpen());

    client.WriteLine(MESSAGE);
    TIMEOUT(static_cast<size_t>(client_get->BytesAvailable())==MESSAGE_LEN,LONG_TIMEOUT);
    std::string msg = client_get->ReadLine();

    ASSERT(msg==MESSAGE_DATA);

    server.Stop();
    client.Close();
    client_get->Close();
}


