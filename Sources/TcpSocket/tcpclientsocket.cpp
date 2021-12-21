#include "tcpclientsocket.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>

#include "socketstatus.h"

TcpClientSocket::TcpClientSocket(int socket)
{
    clientSocket=socket;
}

TcpClientSocket::~TcpClientSocket()
{
    if(clientSocket>0)
    {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket); 
    }
}

void TcpClientSocket::Open(std::string ip, int port)
{
    if(IsOpen())
        Close();

    if(port<=0)
        throw TcpSocketException("Wrong port");;

    in_addr_t ipaddr = inet_addr(ip.c_str());
    if(ipaddr == (in_addr_t)(-1))
        throw TcpSocketException("Wrong host address");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ipaddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket<=0)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }

    if(connect(clientSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }

}

void TcpClientSocket::Close()
{
    const int ERRNO_107 = 107; //Transport endpoint is not connected
    if(clientSocket>0)
    {
        int shutdown_failed=shutdown(clientSocket, SHUT_RDWR);
        if(shutdown_failed)
        {
            if(errno != ERRNO_107)
                TcpSocketException::ThrowErrnoException(errno);
        }

        int close_failed=close(clientSocket);
        if(close_failed)
        {
            if(errno != ERRNO_107)
                TcpSocketException::ThrowErrnoException(errno);
        }
        clientSocket=0;
        std::this_thread::sleep_for(std::chrono::milliseconds((5)));
    }
}

bool TcpClientSocket::IsOpen() const
{
    SocketStatus status(clientSocket);
    return status.isValid();
}

int TcpClientSocket::BytesAvailable() const
{
    SocketStatus status(clientSocket);
    if(status.isValid())
        return status.AvailableBytes();
    return 0;
}

uint64_t TcpClientSocket::Read(char *data, uint64_t size)
{
    const int FLAGS = 0;
    uint64_t result = recv(clientSocket, data, size, FLAGS);
    return result;
}

char TcpClientSocket::ReadByte()
{
    char result = 0;
    int res = Read(&result,1);
    if(res<=0)
        TcpSocketException::ThrowErrnoException(errno);
    return result;
}

std::string TcpClientSocket::ReadLine()
{
    char buffer[MAX_INPUT_BUFFER_SIZE];
    memset(&buffer,0,MAX_INPUT_BUFFER_SIZE);

    char buffer_char = 0;
    int buffer_pos=0;
    do
    {
        int res = Read(&buffer_char, 1);
        if(res>0)
        {
            if(buffer_pos>=MAX_INPUT_BUFFER_SIZE)
            {
                throw TcpSocketException("Buffer overflow");
            }

            buffer[buffer_pos]=buffer_char;
            ++buffer_pos;
        }
        else
        {
            TcpSocketException::ThrowErrnoException(errno);
        }
    }while(buffer_char!='\0' && buffer_char!='\n');

    std::string result(buffer);

    {
        std::string::size_type pos=result.find('\r');
        while(pos!=std::string::npos)
        {
            result.erase(pos,1);
            pos=result.find('\r');
        }
        pos=result.find('\n');
        while(pos!=std::string::npos)
        {
            result.erase(pos,1);
            pos=result.find('\n');
        }
    }

    return result;
}

uint64_t TcpClientSocket::Write(const char *data, uint64_t size)
{
    int sended_bytes = send(clientSocket, data, size, 0);
    return sended_bytes;
}

void TcpClientSocket::WriteLine(std::string data)
{
    int sended_bytes = Write(data.c_str(), data.length());
    if(sended_bytes<=0)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }
}

int TcpClientSocket::Id() const
{
    return clientSocket;
}



