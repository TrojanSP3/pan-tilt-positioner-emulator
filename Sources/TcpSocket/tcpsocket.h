#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include <stdint.h>
#include <unistd.h>

class TcpSocket
{
public:
    TcpSocket(int descriptor=0);
    void Create();
    void Connect(std::string ip, uint16_t port);
    void Bind(const uint16_t port);
    void Listen();
    TcpSocket Accept();
    ssize_t Read(char *data, uint64_t size);
    ssize_t Write(const char *data, uint64_t size);
    void Close(bool noexception=false);

    int Id();
    bool IsSocketOpen();
    bool IsSocketError();
    bool IsTcpConnectionOpen();

    int AvailableBytes();
protected:
    void UpdateStatus();
    void GetStatusByPoll();
    void GetStatusByIoctl();
    void GetStatusByGetsockopt();
private:
    int descriptor;
    bool closed;
    bool readable;
    bool writable;
    bool errors;
    int bytes;
};

#endif // TCPSOCKET_H
