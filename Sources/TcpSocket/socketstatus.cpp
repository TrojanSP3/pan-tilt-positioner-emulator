#include "socketstatus.h"

#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>

SocketStatus::SocketStatus(int socket)
{
    this->socket=socket;
    closed = false;
    readable = false;
    writable = false;
    errors = true;
    bytes = 0;
    Update();
}

void SocketStatus::Update()
{
    if(socket<=0)
        return;
    ByPoll();
    ByIoctl();
    ByGetsockopt();
}

void SocketStatus::ByPoll()
{
    const int POLLING_TIMEOUT_MS=250;
    pollfd fileDescriptor;
    fileDescriptor.fd = socket;
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

void SocketStatus::ByIoctl()
{
    int result=0;
    int ioctl_result = ioctl(socket,FIONREAD,&result);
    if(ioctl_result == 0 )
        bytes = result;
}

void SocketStatus::ByGetsockopt()
{
    tcp_info info;
    unsigned int size_info = sizeof(info);
    int result = getsockopt(socket, SOL_TCP, TCP_INFO, &info, &size_info);
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

bool SocketStatus::isValid() const
{
    bool result = (socket>0) && (!closed) && (!errors);
    return result;
}

bool SocketStatus::isReadable() const
{
    return isValid() && readable;
}

bool SocketStatus::isWritable() const
{
    return isValid() && writable;
}

bool SocketStatus::isClosed() const
{
    return closed;
}

bool SocketStatus::isError() const
{
    return errors;
}

int SocketStatus::AvailableBytes() const
{
    return bytes;
}
