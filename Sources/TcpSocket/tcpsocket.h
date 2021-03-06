#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include "../crossplatform.h"
#include "socketstatus.h"
#include "tcpsocketexception.h"

class TcpSocket
{
public:
    TcpSocket(int descriptor=0);
    void Create();
    void Connect(std::string ip, uint16_t port);
    void Bind(const uint16_t port);
    void Listen();
    TcpSocket Accept();
    ssize_t Read(char *data, size_t size);
    ssize_t Write(const char *data, size_t size);
    void Close(bool noexception=false);

    int Id();
    SocketStatus GetStatus();

protected:
    void GetStatusByPoll();
    void GetStatusByIoctl();
    void GetStatusByGetsockopt();
private:
    int descriptor;

};

#endif // TCPSOCKET_H
