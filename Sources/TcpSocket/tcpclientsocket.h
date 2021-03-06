#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H
#include "tcpsocket.h"

class TcpClientSocket
{
public:
    TcpClientSocket();
    TcpClientSocket(TcpSocket socket);
    ~TcpClientSocket();

    void Open(std::string _ip, uint16_t _port);
    void Close();
    bool IsOpen();
    int BytesAvailable();

    ssize_t Read(char *data, size_t size);
    char ReadByte();
    std::string ReadLine();
    ssize_t Write(const char *data, size_t size);
    void WriteLine(std::string data);
    int Id();
private:
    static const int MAX_INPUT_BUFFER_SIZE=4096;
    std::string ReadString(bool crlf);
    TcpSocket clientSocket;
};

#endif // TCPCLIENTSOCKET_H
