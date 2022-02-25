#include "tcpsocket.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>

#include "tcpsocketexception.h"

const int ERRNO_107 = 107; //Transport endpoint is not connected
const int MAX_PENDING_CONNECTIONS = 8;

TcpSocket::TcpSocket(int descriptor)
{
    this->descriptor=descriptor;
}

void TcpSocket::Create()
{
    descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(descriptor==-1)
    {
        throw TcpSocketException::CreateErrnoException(errno);
    }
}

void TcpSocket::Connect(std::string ip, uint16_t port)
{
    if(port<=0)
        throw TcpSocketException("Wrong port");;

    in_addr_t ipaddr = inet_addr(ip.c_str());
    if(ipaddr == static_cast<in_addr_t>(-1))
        throw TcpSocketException("Wrong host address");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ipaddr;

    if(connect(descriptor, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        throw TcpSocketException::CreateErrnoException(errno);
    }
}

void TcpSocket::Bind(const uint16_t port)
{
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( port );
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int not_binded = bind(descriptor,reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress));
    if(not_binded)
    {
        throw TcpSocketException::CreateErrnoException(errno);
    }
}

void TcpSocket::Listen()
{
    int not_listening = listen(descriptor, MAX_PENDING_CONNECTIONS);
    if(not_listening)
    {
        throw TcpSocketException::CreateErrnoException(errno);
    }
}

TcpSocket TcpSocket::Accept()
{
    struct sockaddr_in client_address;
    socklen_t addrlength = sizeof (struct sockaddr_in);
    int client_socket = accept(descriptor, reinterpret_cast<struct sockaddr *>(&client_address), &addrlength);

    if(client_socket<=0)
    {
        throw TcpSocketException::CreateErrnoException(errno);
    }

    return TcpSocket(client_socket);
}

ssize_t TcpSocket::Read(char *data, uint64_t size)
{
    const int FLAGS = 0;
    ssize_t result = recv(descriptor, data, size, FLAGS);
    return result;
}

ssize_t TcpSocket::Write(const char *data, uint64_t size)
{
    ssize_t sended_bytes = send(descriptor, data, size, 0);
    return sended_bytes;
}

void TcpSocket::Close(bool noexception)
{
    if(descriptor>0)
    {
        int shutdown_failed=shutdown(descriptor, SHUT_RDWR);
        if(shutdown_failed && !noexception)
        {
            if(errno != ERRNO_107)
                throw TcpSocketException::CreateErrnoException(errno);
        }

        int close_failed=close(descriptor);
        if(close_failed  && !noexception)
        {
            if(errno != ERRNO_107)
                throw TcpSocketException::CreateErrnoException(errno);
        }
        descriptor=0;
    }
}

int TcpSocket::Id()
{
    return descriptor;
}

SocketStatus TcpSocket::GetStatus()
{
    return SocketStatus(descriptor);
}
