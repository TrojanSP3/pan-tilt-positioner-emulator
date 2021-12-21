#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include "tcpsocketexception.h"

class TcpClientSocket
{
public:
    TcpClientSocket(int socket=0);
    ~TcpClientSocket();

    void Open(std::string _ip, int _port);
    void Close();
    bool IsOpen() const;
    int BytesAvailable() const;

    uint64_t Read(char *data, uint64_t size);
    char ReadByte();
    std::string ReadLine();
    uint64_t Write(const char *data, uint64_t size);
    void WriteLine(std::string data);
    int Id() const;
private:
    static const int MAX_INPUT_BUFFER_SIZE=4096;
    std::string ReadString(bool crlf);
    int clientSocket;
};

#endif // TCPCLIENTSOCKET_H
