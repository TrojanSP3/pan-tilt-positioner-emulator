#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include <memory>

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
    std::shared_ptr<TcpClientSocket> AcceptClient();//Blockable
private:
    TcpSocket serverSocket;
};

#endif // TCPSERVER_H
