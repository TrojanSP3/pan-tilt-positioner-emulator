#include "tcpclientsocket.h"

#include <cstring>

TcpClientSocket::TcpClientSocket()
{

}

TcpClientSocket::TcpClientSocket(TcpSocket socket)
{
    clientSocket=socket;
}

TcpClientSocket::~TcpClientSocket()
{
    clientSocket.Close(true);
}

void TcpClientSocket::Open(std::string ip, uint16_t port)
{
    clientSocket.Close();
    clientSocket.Create();
    clientSocket.Connect(ip,port);
}

void TcpClientSocket::Close()
{
    clientSocket.Close();
}

bool TcpClientSocket::IsOpen()
{
    SocketStatus status = clientSocket.GetStatus();
    return !status.isClosed();
}

int TcpClientSocket::BytesAvailable()
{
    SocketStatus status = clientSocket.GetStatus();
    return status.AvailableBytes();
}

ssize_t TcpClientSocket::Read(char *data, uint64_t size)
{
    return clientSocket.Read(data,size);
}

char TcpClientSocket::ReadByte()
{
    char result = 0;
    ssize_t res = Read(&result,1);
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
        ssize_t res = Read(&buffer_char, 1);
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

ssize_t TcpClientSocket::Write(const char *data, uint64_t size)
{
    return clientSocket.Write(data,size);
}

void TcpClientSocket::WriteLine(std::string data)
{
    ssize_t sended_bytes = Write(data.c_str(), data.length());
    if(sended_bytes<=0)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }
}

int TcpClientSocket::Id()
{
    return clientSocket.Id();
}



