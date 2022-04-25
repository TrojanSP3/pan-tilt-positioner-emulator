#include "crossplatform.h"

#ifdef WINDOWS_PLATFORM
#include <winsock2.h>
#else
#endif

#include <iostream>

void PlatformInitialization()
#ifdef WINDOWS_PLATFORM
{
	WORD wVersionRequested;
    WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int result_code = WSAStartup(wVersionRequested, &wsaData);;
	switch(result_code)
	{
		case 0:
		break;

		case WSASYSNOTREADY:
			std::cerr<<"WSASYSNOTREADY"<<std::endl;
			std::abort();
		break;
		case WSAVERNOTSUPPORTED:
			std::cerr<<"WSAVERNOTSUPPORTED"<<std::endl;
			std::abort();
		break;
		case WSAEINPROGRESS:
			std::cerr<<"WSAEINPROGRESS"<<std::endl;
			std::abort();
		break;
		case WSAEPROCLIM:
			std::cerr<<"WSAEPROCLIM"<<std::endl;
			std::abort();
		break;
		case WSAEFAULT:
			std::cerr<<"WSAEFAULT"<<std::endl;
			std::abort();
		break;
		default:
			std::cerr<<"PlatformInitialization() error"<<std::endl;
			std::abort();
		break;
	}
}
#else
{

}
#endif 