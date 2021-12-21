#include "tcpserversocket.h"

#include <cstring>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "socketstatus.h"

const int ERRNO_107 = 107; //Transport endpoint is not connected

TcpServerSocket::TcpServerSocket()
{
	port=0;
	serverSocket=0;
}

TcpServerSocket::~TcpServerSocket()
{
    if(serverSocket>0)
    {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
    }
}

void TcpServerSocket::Start(const int port)
{
    Stop();
    this->port=port;
    SetUpServerSocket();
}

void TcpServerSocket::SetUpServerSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serverSocket<=0)
    {
        ThrowException("Error on opening server socket",errno);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( port );
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int not_binded = bind(serverSocket,(struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if(not_binded)
    {
        int remember_errno = errno;
        Stop();
        ThrowException("Error on binding server socket",remember_errno);
    }

    int not_listening = listen(serverSocket, MAX_PENDING_CONNECTIONS);
    if(not_listening)
    {
        int remember_errno = errno;
        Stop();
        ThrowException("Error on listening server socket",remember_errno);
    }
}

void TcpServerSocket::Stop()
{
    if(serverSocket>0)
    {
        int shutdown_failed=shutdown(serverSocket, SHUT_RDWR);
        if(shutdown_failed)
        {
            if(errno != ERRNO_107)
            {
                ThrowException("Error on shutdown server socket",errno);
            }
        }


        int close_failed=close(serverSocket);
        if(close_failed)
        {
            if(errno != ERRNO_107)
            {
                ThrowException("Error on closing server socket",errno);
            }
        }

        serverSocket=0;
        std::this_thread::sleep_for(std::chrono::milliseconds((5)));
    }
}

void TcpServerSocket::ThrowException(std::string msg, int _errno)
{
    std::string result = msg;
    if(_errno)
    {
        std::string errorString = TcpSocketException::ErrnoToString(_errno);
        result+=". Errno: "+errorString;
    }
    throw TcpSocketException(result);
}

bool TcpServerSocket::IsListening() const
{
    if(serverSocket>0)
    {
        SocketStatus status(serverSocket);
        return status.isValid();
    }
    else
    {
        return false;
    }
}

bool TcpServerSocket::IsClientAvailable() const
{
    SocketStatus status(serverSocket);
    bool result = !status.isError() && status.isReadable();
    return result;
}

TcpClientSocket* TcpServerSocket::AcceptClient()
{
    struct sockaddr_in client_address;
    int addrlength = sizeof (struct sockaddr_in);
    int client_socket = accept(serverSocket, (struct sockaddr*)&client_address, (socklen_t*)&addrlength);

    if(client_socket<=0)
    {
        ThrowException("Error on accepting client socket",errno);
    }

    SetClientSocketParameters(client_socket);
    TcpClientSocket* client=new TcpClientSocket(client_socket);
    return client;
}

void TcpServerSocket::SetClientSocketParameters(int client_socket)
{
    const int opt_length = sizeof(int);

    const int opt_keepalive_on = 1;
    const int opt_keepcnt = 1;
    const int opt_keepidle = 1;
    const int opt_keepinterval = 1;
    const int opt_user_timeout = (opt_keepidle + opt_keepinterval * opt_keepcnt) * 1000;
    int errors = 0;

    errors = setsockopt(client_socket, SOL_TCP, TCP_USER_TIMEOUT, &opt_user_timeout, opt_length);
    if(errors)
    {
        ThrowException("Error on parameter: TCP_USER_TIMEOUT",errno);
    }

    errors = setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &opt_keepalive_on, opt_length);
    if(errors)
    {
        ThrowException("Error on parameter: SO_KEEPALIVE",errno);
    }

    errors = setsockopt(client_socket, SOL_TCP, TCP_KEEPCNT, &opt_keepcnt, opt_length);
    if(errors)
    {
        ThrowException("Error on parameter: TCP_KEEPCNT",errno);
    }

    errors = setsockopt(client_socket, SOL_TCP, TCP_KEEPIDLE, &opt_keepidle, opt_length);
    if(errors)
    {
        ThrowException("Error on parameter: TCP_KEEPIDLE",errno);
    }

    errors = setsockopt(client_socket, SOL_TCP, TCP_KEEPINTVL, &opt_keepinterval, opt_length);
    if(errors)
    {
        ThrowException("Error on parameter: TCP_KEEPINTVL",errno);
    }
}



