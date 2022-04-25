#include "tcpsocketexception.h"
#include "../crossplatform.h"

#include <cstring>

#ifdef WINDOWS_PLATFORM
	#include <winsock2.h>
#else

#endif

std::string TcpSocketException::ErrorToString(int code)
{
#ifdef WINDOWS_PLATFORM
	LPTSTR result;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
        code,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&result,
        0,
        NULL);
	return result;
#else
	std::string result(strerror(code));
    if(result.empty())
        result="Unknown error: "+std::to_string(code);
    result=std::to_string(code)+": "+result;
    return result;
#endif
    
}

int TcpSocketException::GetLastErrorCode()
{
#ifdef WINDOWS_PLATFORM
	return WSAGetLastError();
#else
	return errno;
#endif
}

TcpSocketException TcpSocketException::CreateErrorException(int code)
{
    std::string errorString = TcpSocketException::ErrorToString(code);
    return TcpSocketException(errorString);
}

TcpSocketException::TcpSocketException(std::string msg)
{
    message="TcpSocketException: "+msg;
}

const char* TcpSocketException::what() const NOEXCEPT
{
    return message.c_str();
}
