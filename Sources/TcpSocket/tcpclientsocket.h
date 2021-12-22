#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include "tcpsocketexception.h"

class TcpClientSocket
{
public:
    TcpClientSocket(int socket=0);
    ~TcpClientSocket();

    void Open(std::string _ip, uint16_t _port);
    void Close();
    bool IsOpen() const;
    int BytesAvailable() const;

    ssize_t Read(char *data, uint64_t size);
    char ReadByte();
    std::string ReadLine();
    ssize_t Write(const char *data, uint64_t size);
    void WriteLine(std::string data);
    int Id() const;
private:
    static const int MAX_INPUT_BUFFER_SIZE=4096;
    std::string ReadString(bool crlf);
    int clientSocket;
};

#endif // TCPCLIENTSOCKET_H
