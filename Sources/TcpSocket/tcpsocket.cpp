#include "tcpsocket.h"
#include "../crossplatform.h"
#include "tcpsocketexception.h"

#ifdef WINDOWS_PLATFORM
#include <winsock2.h>
inline int close(_In_ SOCKET s)
{
	return closesocket(s);
}
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

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
		int error_code = TcpSocketException::GetLastErrorCode();
		throw TcpSocketException::CreateErrorException(error_code);
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
		int error_code = TcpSocketException::GetLastErrorCode();
		throw TcpSocketException::CreateErrorException(error_code);
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
		int error_code = TcpSocketException::GetLastErrorCode();
		throw TcpSocketException::CreateErrorException(error_code);
	}
}

void TcpSocket::Listen()
{
    int not_listening = listen(descriptor, MAX_PENDING_CONNECTIONS);
    if(not_listening)
	{
		int error_code = TcpSocketException::GetLastErrorCode();
		throw TcpSocketException::CreateErrorException(error_code);
	}
}

TcpSocket TcpSocket::Accept()
{
    struct sockaddr_in client_address;
    socklen_t addrlength = sizeof (struct sockaddr_in);
    int client_socket = accept(descriptor, reinterpret_cast<struct sockaddr *>(&client_address), &addrlength);

    if(client_socket<=0)
	{
		int error_code = TcpSocketException::GetLastErrorCode();
		throw TcpSocketException::CreateErrorException(error_code);
	}

    return TcpSocket(client_socket);
}

ssize_t TcpSocket::Read(char *data, size_t size)
{
    const int FLAGS = 0;
    ssize_t result = recv(descriptor, data, size, FLAGS);
    return result;
}

ssize_t TcpSocket::Write(const char *data, size_t size)
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
			int error_code = TcpSocketException::GetLastErrorCode();
            if(error_code != ERROR_NO_ENDPOINT)
			{
				throw TcpSocketException::CreateErrorException(error_code);
			}
        }

		int close_failed=close(descriptor);

        if(close_failed  && !noexception)
        {
            int error_code = TcpSocketException::GetLastErrorCode();
            if(error_code != ERROR_NO_ENDPOINT)
			{
				throw TcpSocketException::CreateErrorException(error_code);
			}
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
