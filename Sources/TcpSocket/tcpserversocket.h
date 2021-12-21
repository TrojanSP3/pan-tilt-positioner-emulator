#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include <string>
#include <atomic>
#include <queue>
#include <mutex>

#include "tcpsocketexception.h"
#include "tcpclientsocket.h"

class TcpServerSocket
{
public:
	TcpServerSocket();
    ~TcpServerSocket();
    void Start(const int port);
    void Stop();
    bool IsListening() const;
    bool IsClientAvailable() const;
    TcpClientSocket* AcceptClient();//Blockable
protected:
    void ThrowException(std::string msg, int _errno);
    void SetUpServerSocket();
    void SetClientSocketParameters(int client_socket);
private:
    int port;
    int serverSocket;
private:
    static const int MAX_PENDING_CONNECTIONS = 5;
};

#endif // TCPSERVER_H
