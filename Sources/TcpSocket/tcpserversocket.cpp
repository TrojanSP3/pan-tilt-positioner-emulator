#include "tcpserversocket.h"

TcpServerSocket::TcpServerSocket()
{

}

TcpServerSocket::~TcpServerSocket()
{
    serverSocket.Close(true);
}

void TcpServerSocket::Start(const uint16_t port)
{
    Stop();
    serverSocket.Create();
    serverSocket.Bind(port);
    serverSocket.Listen();
}


void TcpServerSocket::Stop()
{
    serverSocket.Close();
}

bool TcpServerSocket::IsListening()
{
    if(serverSocket.IsSocketOpen())
    {
        return serverSocket.IsTcpConnectionOpen();
    }
    else
    {
        return false;
    }
}

bool TcpServerSocket::IsClientAvailable()
{
    return serverSocket.IsSocketOpen() && serverSocket.IsTcpConnectionOpen() && serverSocket.AvailableBytes()>0;
}

TcpClientSocket* TcpServerSocket::AcceptClient()
{
    TcpSocket descriptor = serverSocket.Accept();
    TcpClientSocket* client=new TcpClientSocket(descriptor);
    return client;
}




