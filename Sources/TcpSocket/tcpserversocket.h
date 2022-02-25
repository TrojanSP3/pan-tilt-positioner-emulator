#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include "tcpclientsocket.h"

class TcpServerSocket
{
public:
	TcpServerSocket();
    ~TcpServerSocket();
    void Start(const uint16_t port);
    void Stop();
    bool IsListening();
    bool IsClientAvailable();
    TcpClientSocket* AcceptClient();//Blockable
private:
    TcpSocket serverSocket;
};

#endif // TCPSERVER_H
