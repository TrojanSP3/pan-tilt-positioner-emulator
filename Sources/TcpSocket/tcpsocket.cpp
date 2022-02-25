#include "tcpsocket.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>

#include "tcpsocketexception.h"

const int ERRNO_107 = 107; //Transport endpoint is not connected
const int MAX_PENDING_CONNECTIONS = 8;

TcpSocket::TcpSocket()
{
    descriptor=0;
    closed=true;
    readable=false;
    writable=false;
    errors=false;
}

TcpSocket::~TcpSocket()
{
    if(descriptor>0)
    {
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
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
        TcpSocketException::ThrowErrnoException(errno);
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
        TcpSocketException::ThrowErrnoException(errno);
    }
}

void TcpSocket::Listen()
{
    int not_listening = listen(descriptor, MAX_PENDING_CONNECTIONS);
    if(not_listening)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }
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

void TcpSocket::Close()
{
    if(descriptor>0)
    {
        int shutdown_failed=shutdown(descriptor, SHUT_RDWR);
        if(shutdown_failed)
        {
            if(errno != ERRNO_107)
                TcpSocketException::ThrowErrnoException(errno);
        }

        int close_failed=close(descriptor);
        if(close_failed)
        {
            if(errno != ERRNO_107)
                TcpSocketException::ThrowErrnoException(errno);
        }
        descriptor=0;
    }
}

bool TcpSocket::IsSocketOpen()
{
    return descriptor>0;
}

bool TcpSocket::IsSocketError()
{
    if(descriptor>0)
    {
        UpdateStatus();
    }
    else
    {
       errors=true;
    }
    return errors;
}

bool TcpSocket::IsTcpConnectionOpen()
{
    if(descriptor>0)
    {
        UpdateStatus();
    }
    else
    {
       closed=true;
       errors=true;
       readable=false;
       writable=false;
    }
    return !closed && !errors && readable && writable;
}


int TcpSocket::AvailableBytes()
{
    if(descriptor>0)
    {
        UpdateStatus();
    }
    else
    {
       bytes=0;
    }
    return bytes;
}

void TcpSocket::Create()
{
    descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(descriptor==-1)
    {
        TcpSocketException::ThrowErrnoException(errno);
    }
}

void TcpSocket::UpdateStatus()
{
    GetStatusByPoll();
    GetStatusByIoctl();
    GetStatusByGetsockopt();
}

void TcpSocket::GetStatusByPoll()
{
    const int POLLING_TIMEOUT_MS=250;
    pollfd fileDescriptor;
    fileDescriptor.fd = descriptor;
    fileDescriptor.events = POLLIN | POLLOUT | POLLPRI;
    fileDescriptor.revents = 0;
    int poll_result = poll(&fileDescriptor,1,POLLING_TIMEOUT_MS);

    short poll_in = fileDescriptor.revents & POLLIN ;
    //short poll_pri = fileDescriptor.revents & POLLPRI ;
    short poll_out = fileDescriptor.revents & POLLOUT ;

    short poll_err = fileDescriptor.revents & POLLERR ;
    short poll_hup = fileDescriptor.revents & POLLHUP ;
    short poll_nval = fileDescriptor.revents & POLLNVAL ;
    short bad_events = poll_err | poll_hup | poll_nval;

    readable = poll_in>0;
    writable = poll_out>0;
    errors = (bad_events>0) || (poll_result<0);
}

void TcpSocket::GetStatusByIoctl()
{
    int result=0;
    int ioctl_result = ioctl(descriptor,FIONREAD,&result);
    if(ioctl_result == 0 )
        bytes = result;
}

void TcpSocket::GetStatusByGetsockopt()
{
    tcp_info info;
    unsigned int size_info = sizeof(info);
    int result = getsockopt(descriptor, SOL_TCP, TCP_INFO, &info, &size_info);
    if(result==0)
    {
        switch(info.tcpi_state)
        {
        case TCP_FIN_WAIT1:
        case TCP_FIN_WAIT2:
        case TCP_TIME_WAIT:
        case TCP_CLOSE:
        case TCP_CLOSE_WAIT:
        case TCP_CLOSING:
            writable = false;
            errors = false;
            closed = true;
            break;
        default: break;
        };
    }
    else
    {
        errors=true;
    }
}



