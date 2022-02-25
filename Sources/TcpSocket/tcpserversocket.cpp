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
    SocketStatus status = serverSocket.GetStatus();
    return status.isValid();
}

bool TcpServerSocket::IsClientAvailable()
{
    SocketStatus status = serverSocket.GetStatus();
    return !status.isError() && status.isReadable();
}

TcpClientSocket* TcpServerSocket::AcceptClient()
{
    TcpSocket descriptor = serverSocket.Accept();
    TcpClientSocket* client=new TcpClientSocket(descriptor);
    return client;
}




